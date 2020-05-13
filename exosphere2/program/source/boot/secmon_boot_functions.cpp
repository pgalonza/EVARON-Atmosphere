/*
 * Copyright (c) 2018-2020 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <exosphere.hpp>
#include "../secmon_error.hpp"
#include "secmon_boot.hpp"
#include "secmon_boot_cache.hpp"
#include "secmon_boot_functions.hpp"
#include "secmon_boot_key_data.hpp"

namespace ams::secmon::boot {

    namespace {

        constexpr inline uintptr_t SYSCTR0 = MemoryRegionVirtualDeviceSysCtr0.GetAddress();

        NOINLINE void DecryptPayload(uintptr_t dst, uintptr_t src, size_t size, const void *iv, size_t iv_size, u8 key_generation) {
            secmon::boot::DecryptPackage2(reinterpret_cast<void *>(dst), size, reinterpret_cast<void *>(src), size, Package2AesKey, util::size(Package2AesKey), iv, iv_size, key_generation);
        }

    }

    void CheckVerifyResult(bool verify_result, pkg1::ErrorInfo error_info, const char *message) {
        if (!verify_result) {
            secmon::SetError(error_info);
            AMS_ABORT(message);
        }
    }

    void ClearIram() {
        /* Clear the boot code image from where it was loaded in IRAM. */
        util::ClearMemory(MemoryRegionPhysicalIramBootCodeImage.GetPointer(), MemoryRegionPhysicalIramBootCodeImage.GetSize());
    }

    void WaitForNxBootloader(const pkg1::SecureMonitorParameters &params, pkg1::BootloaderState state) {
        /* Check NX Bootloader's state once per microsecond until it's advanced enough. */
        while (params.bootloader_state < state) {
            util::WaitMicroSeconds(1);
        }
    }

    void LoadBootConfig(const void *src) {
        pkg1::BootConfig * const dst = secmon::impl::GetBootConfigStorage();

        if (pkg1::IsProduction()) {
            std::memset(dst, 0, sizeof(*dst));
        } else {
            hw::FlushDataCache(src, sizeof(*dst));
            hw::DataSynchronizationBarrierInnerShareable();
            std::memcpy(dst, src, sizeof(*dst));
        }
    }

    void VerifyOrClearBootConfig() {
        /* On production hardware, the boot config is already cleared. */
        if (pkg1::IsProduction()) {
            return;
        }

        pkg1::BootConfig * const bc = secmon::impl::GetBootConfigStorage();

        /* Determine if the bc is valid for the device. */
        bool valid_for_device = false;
        {
            const bool valid_signature = secmon::boot::VerifyBootConfigSignature(*bc, BootConfigRsaPublicModulus, util::size(BootConfigRsaPublicModulus));
            if (valid_signature) {
                valid_for_device = secmon::boot::VerifyBootConfigEcid(*bc);
            }
        }

        /* If the boot config is not valid for the device, clear its signed data. */
        if (!valid_for_device) {
            util::ClearMemory(std::addressof(bc->signed_data), sizeof(bc->signed_data));
        }
    }

    void EnableTsc(u64 initial_tsc_value) {
        /* Write the initial value to the CNTCV registers. */
        const u32 lo = static_cast<u32>(initial_tsc_value >>  0);
        const u32 hi = static_cast<u32>(initial_tsc_value >> 32);

        reg::Write(SYSCTR0 + SYSCTR0_CNTCV0, lo);
        reg::Write(SYSCTR0 + SYSCTR0_CNTCV1, hi);

        /* Configure the system counter control register. */
        reg::Write(SYSCTR0 + SYSCTR0_CNTCR, SYSCTR0_REG_BITS_ENUM(CNTCR_HDBG, ENABLE),
                                            SYSCTR0_REG_BITS_ENUM(CNTCR_EN,   ENABLE));
    }

    void WriteGpuCarveoutMagicNumbers() {
        /* Define the magic numbers. */
        constexpr u32 GpuMagicNumber       = 0xC0EDBBCC;
        constexpr u32 SkuInfo              = 0x83;
        constexpr u32 HdcpMicroCodeVersion = 0x2;
        constexpr u32 ChipIdErista         = 0x210;
        constexpr u32 ChipIdMariko         = 0x214;

        /* Get our pointers. */
        u32 *gpu_magic  = MemoryRegionDramGpuCarveout.GetEndPointer<u32>() - (0x004 / sizeof(*gpu_magic));
        u32 *tsec_magic = MemoryRegionDramGpuCarveout.GetEndPointer<u32>() - (0x100 / sizeof(*tsec_magic));

        /* Write the gpu magic number. */
        gpu_magic[0] = GpuMagicNumber;

        /* Write the tsec magic numbers. */
        tsec_magic[0] = SkuInfo;
        tsec_magic[1] = HdcpMicroCodeVersion;
        tsec_magic[2] = (false /* TODO: IsMariko */) ? ChipIdMariko : ChipIdErista;

        /* Flush the magic numbers. */
        hw::FlushDataCache(gpu_magic,  1 * sizeof(u32));
        hw::FlushDataCache(tsec_magic, 3 * sizeof(u32));
        hw::DataSynchronizationBarrierInnerShareable();
    }

    void UpdateBootConfigForPackage2Header(const pkg2::Package2Header &header) {
        /* Check for all-zeroes signature. */
        bool is_decrypted = header.signature[0] == 0;
        is_decrypted &= crypto::IsSameBytes(header.signature, header.signature + 1, sizeof(header.signature) - 1);

        /* Check for valid magic. */
        is_decrypted &= crypto::IsSameBytes(header.meta.magic, pkg2::Package2Meta::Magic::String, sizeof(header.meta.magic));

        /* Set the setting in boot config. */
        secmon::impl::GetBootConfigStorage()->signed_data.SetPackage2Decrypted(is_decrypted);
    }

    void VerifyPackage2HeaderSignature(pkg2::Package2Header &header, bool verify) {
        if (pkg1::IsProductionForPublicKey()) {
            CheckVerifyResult(secmon::boot::VerifyPackage2Signature(header, Package2RsaPublicModulusProduction,  util::size(Package2RsaPublicModulusProduction)),  pkg1::ErrorInfo_InvalidPackage2Signature, "package2 header sign verification failed");
        } else if (verify) {
            CheckVerifyResult(secmon::boot::VerifyPackage2Signature(header, Package2RsaPublicModulusDevelopment, util::size(Package2RsaPublicModulusDevelopment)), pkg1::ErrorInfo_InvalidPackage2Signature, "package2 header sign verification failed");
        }
    }

    void DecryptPackage2Header(pkg2::Package2Meta *dst, const pkg2::Package2Meta &src, bool encrypted) {
        if (encrypted) {
            constexpr int IvSize = 0x10;

            /* Decrypt the header. */
            DecryptPackage2(dst, sizeof(*dst), std::addressof(src), sizeof(src), Package2AesKey, util::size(Package2AesKey), std::addressof(src), IvSize, src.GetKeyGeneration());

            /* Copy back the iv, which encodes encrypted metadata. */
            std::memcpy(dst, std::addressof(src), IvSize);
        } else {
            std::memcpy(dst, std::addressof(src), sizeof(*dst));
        }
    }

    void VerifyPackage2Header(const pkg2::Package2Meta &meta) {
        /* Validate the metadata. */
        CheckVerifyResult(VerifyPackage2Meta(meta),    pkg1::ErrorInfo_InvalidPackage2Meta, "package2 meta verification failed");

        /* Validate the version. */
        CheckVerifyResult(VerifyPackage2Version(meta), pkg1::ErrorInfo_InvalidPackage2Version, "package2 version verification failed");
    }

    void DecryptAndLoadPackage2Payloads(uintptr_t dst, const pkg2::Package2Meta &meta, uintptr_t src, bool encrypted) {
        /* Get the key generation for crypto. */
        const u8 key_generation = meta.GetKeyGeneration();
        /* Decrypt or load each payload in order. */
        for (int i = 0; i < pkg2::PayloadCount; ++i) {
            if (encrypted) {
                DecryptPayload(dst + meta.payload_offsets[i], src, meta.payload_sizes[i], meta.payload_ivs[i], sizeof(meta.payload_ivs[i]), key_generation);
            } else {
                std::memcpy(reinterpret_cast<void *>(dst + meta.payload_offsets[i]), reinterpret_cast<void *>(src), meta.payload_sizes[i]);
            }

            src += meta.payload_sizes[i];
        }
    }

}