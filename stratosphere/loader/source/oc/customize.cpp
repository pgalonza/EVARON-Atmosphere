/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
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

#include "customize.hpp"

namespace ams::ldr::oc {

//volatile EristaMtcTable EristaMtcTablePlaceholder = { .rev = ERISTA_MTC_MAGIC, };
//volatile MarikoMtcTable MarikoMtcTablePlaceholder = { .rev = MARIKO_MTC_MAGIC, };

volatile CustomizeTable C = {
/* DRAM Timing:
 * AUTO_ADJ_ALL: Auto adjust timings for Mariko LPDDR4X ≤3733 Mbps specs, 8Gb density. (Default)
 * CUSTOM_ADJ_ALL: Basically same as NO_ADJ_ALL, with core timing adjustments
 * NO_ADJ_ALL: No timing adjustment for both Erista and Mariko.
 * CUSTOMIZED_ALL: Replace with values in customized table for both Erista and Mariko.
 */
.mtcConf = AUTO_ADJ_ALL,

/* Common:
 * - Boost Clock in kHz:
 *   Default: 1785000
 *   Boost clock will be applied when applications request higher CPU frequency for quicker loading.
 *   This will be set regardless of whether sys-clk is enabled.
 */
.commonCpuBoostClock = 1785000,
/* - EMC Vddq (Erista Only) and RAM Vdd2 Voltage in uV
 *   Range: 1100'000 to 1250'000 uV
 *   Erista Default(HOS): 1125'000 (bootloader: 1100'000)
 *   Mariko Default: 1100'000 (It will not work without sys-clk-OC.)
 *   Value should be divided evenly by 12'500.
 *   Not enabled by default.
 */
.commonEmcMemVolt  = 0,

/* Erista CPU:
 * - Max Voltage in mV
 * - CpuVoltL4T: 1235
 */
.eristaCpuMaxVolt  = 1235,

/* Erista EMC(RAM):
 * - RAM Clock in kHz
 *   [WARNING]
 *   RAM overclock could be UNSTABLE if timing parameters are not suitable for your DRAM:
 *   - Graphical glitches
 *   - System instabilities
 *   - NAND corruption
 */
.eristaEmcMaxClock = 1862400,

/* Mariko CPU:
 * - Max Voltage in mV:
 *   Default voltage: 1120
 */
.marikoCpuMaxVolt    = 1235,

/* Mariko EMC(RAM):
 * - RAM Clock in kHz:
 *   Values should be ≥ 1600000, and divided evenly by 9600.
 *   [WARNING]
 *   RAM overclock could be UNSTABLE if timing parameters are not suitable for your DRAM:
 *   - Graphical glitches
 *   - System instabilities
 *   - NAND corruption
 */
.marikoEmcMaxClock = 1996800,
/* - EMC Vddq (Mariko Only) Voltage in uV
 *   Range: 550'000 to 650'000 uV
 *   Value should be divided evenly by 5'000
 *   Default: 600'000
 *   Not enabled by default.
 *   This will not work without sys-clk-OC.
 */
.marikoEmcVddqVolt = 0,

.marikoCpuUV = 0,

.marikoGpuUV = 0,

.commonGpuVoltOffset = 0,

.marikoEmcDvbShift = 0,

.ramTimingPresetOne = 0,

.ramTimingPresetTwo = 0,

.ramTimingPresetThree = 0,

.ramTimingPresetFour = 0,

.ramTimingPresetFive = 0,

.ramTimingPresetSix = 0,

.ramTimingPresetSeven = 0,

.marikoGpuVoltArray = {635, 635, 635, 635, 635, 635, 635, 635, 635, 635, 635, 635, 660, 685, 715, 745, 765, 785},

/* Advanced Settings:
 * - Erista CPU DVFS Table:
 */
.eristaCpuDvfsTable = {
    {  204000, {  721094 }, {} },
    {  306000, {  754040 }, {} },
    {  408000, {  786986 }, {} },
    {  510000, {  819932 }, {} },
    {  612000, {  852878 }, {} },
    {  714000, {  885824 }, {} },
    {  816000, {  918770 }, {} },
    {  918000, {  951716 }, {} },
    { 1020000, {  984662 }, { -2875621,  358099, -8585 } },
    { 1122000, { 1017608 }, {   -52225,  104159, -2816 } },
    { 1224000, { 1050554 }, {  1076868,    8356,  -727 } },
    { 1326000, { 1083500 }, {  2208191,  -84659,  1240 } },
    { 1428000, { 1116446 }, {  2519460, -105063,  1611 } },
    { 1581000, { 1130000 }, {  2889664, -122173,  1834 } },
    { 1683000, { 1168000 }, {  5100873, -279186,  4747 } },
    { 1785000, { 1227500 }, {  5100873, -279186,  4747 } },
    // Appending table
    { 1887000, { 1235000 }, {  5100873, -279186,  4747 } },
    { 1963500, { 1235000 }, {  5100873, -279186,  4747 } },
    { 2091000, { 1235000 }, {  5100873, -279186,  4747 } },
},

/* - Mariko CPU DVFS Table:
 * 2397000 might not work for some SoCs.
 */
.marikoCpuDvfsTable = {
    {  204000, {  721589, -12695, 27 }, {} },
    {  306000, {  747134, -14195, 27 }, {} },
    {  408000, {  776324, -15705, 27 }, {} },
    {  510000, {  809160, -17205, 27 }, {} },
    {  612000, {  845641, -18715, 27 }, {} },
    {  714000, {  885768, -20215, 27 }, {} },
    {  816000, {  929540, -21725, 27 }, {} },
    {  918000, {  976958, -23225, 27 }, {} },
    { 1020000, { 1028021, -24725, 27 }, { 1120000 } },
    { 1122000, { 1082730, -26235, 27 }, { 1120000 } },
    { 1224000, { 1141084, -27735, 27 }, { 1120000 } },
    { 1326000, { 1203084, -29245, 27 }, { 1120000 } },
    { 1428000, { 1268729, -30745, 27 }, { 1120000 } },
    { 1581000, { 1374032, -33005, 27 }, { 1120000 } },
    { 1683000, { 1448791, -34505, 27 }, { 1120000 } },
    { 1785000, { 1527196, -36015, 27 }, { 1120000 } },
    { 1887000, { 1609246, -37515, 27 }, { 1120000 } },
    { 1963500, { 1675751, -38635, 27 }, { 1120000 } },
    // Appending table
    { 2091000, { 1716501, -39395, 27 }, { 1235000 } },
    { 2193000, { 1775132, -40505, 27 }, { 1235000 } },
    { 2295000, { 1866287, -42005, 27 }, { 1235000 } },
    { 2397000, { 1961107, -43506, 27 }, { 1235000 } },
},

.marikoCpuDvfsTableSLT = {
    {  204000, {  732856,   -17335,      113 }, {} },
    {  306000, {  760024,   -18195,      113 }, {} },
    {  408000, {  789258,   -19055,      113 }, {} },
    {  510000, {  789258,   -19055,      113 }, {} },
    {  612000, {  853926,   -20775,      113 }, {} },
    {  714000, {  889361,   -21625,      113 }, {} },
    {  816000, {  926862,   -22485,      113 }, {} },
    {  918000, {  966431,   -23345,      113 }, {} },
    { 1020000, { 1008066,   -24205,      113 }, { 1120000 } },
    { 1122000, { 1051768,   -25065,      113 }, { 1120000 } },
    { 1224000, { 1097537,   -25925,      113 }, { 1120000 } },
    { 1326000, { 1145373,   -26785,      113 }, { 1120000 } },
    { 1428000, { 1195276,   -27645,      113 }, { 1120000 } },
    { 1581000, { 1274006,   -28935,      113 }, { 1120000 } },
    { 1683000, { 1329076,   -29795,      113 }, { 1120000 } },
    { 1785000, { 1386213,   -30655,      113 }, { 1120000 } },
    { 1887000, { 1445416,   -31515,      113 }, { 1120000 } },
    { 1963500, { 1490873,   -32155,      113 }, { 1120000 } },
    // Appending table
    { 2091000, { 1580725,   -33235,      113 }, { 1120000 } },
    { 2193000, { 1580725,   -33235,      113 }, { 1235000 } },
    { 2295000, { 1635431,   -34095,      113 }, { 1235000 } },
    { 2397000, { 1702903,   -34955,      113 }, { 1235000 } },
    { 2499000, { 1754400,   -35643,      113 }, { 1235000 } },
    { 2601000, { 1805897,   -36331,      113 }, { 1235000 } },
	// { 2703000, { 1857394,   -37019,      113 }, { 1235000 } },
	// { 2805000, { 1908891,   -37707,      113 }, { 1235000 } },
	// { 2907000, { 1960388,   -38395,      113 }, { 1250000 } },
},

/* - Erista     { 1305600, {}, { 1374130, -13725, -859,    0,   4442, 576 } },GPU DVFS Table:
 */
.eristaGpuDvfsTable = {
    {   76800, { }, {  814294, 8144, -940, 808, -21583, 226 } },
    {  153600, { }, {  856185, 8144, -940, 808, -21583, 226 } },
    {  230400, { }, {  898077, 8144, -940, 808, -21583, 226 } },
    {  307200, { }, {  939968, 8144, -940, 808, -21583, 226 } },
    {  384000, { }, {  981860, 8144, -940, 808, -21583, 226 } },
    {  460800, { }, { 1023751, 8144, -940, 808, -21583, 226 } },
    {  537600, { }, { 1065642, 8144, -940, 808, -21583, 226 } },
    {  614400, { }, { 1107534, 8144, -940, 808, -21583, 226 } },
    {  691200, { }, { 1149425, 8144, -940, 808, -21583, 226 } },
    {  768000, { }, { 1191317, 8144, -940, 808, -21583, 226 } },
    {  844800, { }, { 1233208, 8144, -940, 808, -21583, 226 } },
    {  921600, { }, { 1275100, 8144, -940, 808, -21583, 226 } },
    {  998400, { }, { 1316991, 8144, -940, 808, -21583, 226 } },
},

/* - Mariko GPU DVFS Table:
 * 1305600 might not work for some SoCs.
 */
.marikoGpuDvfsTable = {
    {   76800, {}, {  610000,                                 } },
    {  153600, {}, {  610000,                                 } },
    {  230400, {}, {  610000,                                 } },
    {  307200, {}, {  610000,                                 } },
    {  384000, {}, {  610000,                                 } },
    {  460800, {}, {  610000,                                 } },
    {  537600, {}, {  801688, -10900, -163,  298, -10599, 162 } },
    {  614400, {}, {  824214,  -5743, -452,  238,  -6325,  81 } },
    {  691200, {}, {  848830,  -3903, -552,  119,  -4030,  -2 } },
    {  768000, {}, {  891575,  -4409, -584,    0,  -2849,  39 } },
    {  844800, {}, {  940071,  -5367, -602,  -60,    -63, -93 } },
    {  921600, {}, {  986765,  -6637, -614, -179,   1905, -13 } },
    {  998400, {}, { 1098475, -13529, -497, -179,   3626,   9 } },
    { 1075200, {}, { 1163644, -12688, -648,    0,   1077,  40 } },
    { 1152000, {}, { 1204812,  -9908, -830,    0,   1469, 110 } },
    { 1228800, {}, { 1277303, -11675, -859,    0,   3722, 313 } },
    { 1267200, {}, { 1335531, -12567, -867,    0,   3681, 559 } },
    // Appending table
    // { 1305600, {}, { 1374130, -13725, -859,    0,   4442, 576 } },
},

.marikoGpuDvfsTableSLT = {
    {   76800, {},  {  635000,                                  } },
    {  153600, {},  {  635000,                                  } },
    {  230400, {},  {  635000,                                  } },
    {  307200, {},  {  635000,                                  } },
    {  384000, {},  {  635000,                                  } },
    {  460800, {},  {  635000,                                  } },
    {  537600, {},  {  635000,                                  } },
    {  614400, {},  {  635000,                                  } },
    {  691200, {},  {  635000,                                  } },
    {  768000, {},  {  635000,                                  } },
    {  844800, {},  {  635000,                                  } },
    {  921600, {},  {  635000,                                  } },
    {  998400, {},  {  901575,  -4409, -584,    0,  -2849,   39 } },
    { 1075200, {},  {  940071,  -5367, -602,  -60,    -63,  -93 } },
    { 1152000, {},  {  996765,  -6637, -614, -179,   1905,  -13 } },
    { 1228800, {},  { 1098475, -13529, -497, -179,   3626,    9 } },
    { 1267200, {},  { 1131060, -13109, -573,  -90,   2352,   25 } },
    { 1305600, {},  { 1163644, -12688, -648,    0,   1077,   40 } },
},
.marikoGpuDvfsTableHiOPT = {
    	{   76800, {},  {  550000,                                  } },
    	{  153600, {},  {  550000,                                  } },
    	{  230400, {},  {  550000,                                  } },
    	{  307200, {},  {  550000,                                  } },
    	{  384000, {},  {  550000,                                  } },
    	{  460800, {},  {  550000,                                  } },
    	{  537600, {},  {  550000,                                  } },
    	{  614400, {},  {  550000,                                  } },
    	{  691200, {},  {  550000,                                  } },
    	{  768000, {},  {  801688, -10900, -163,  298, -10599,  162 } },
    	{  844800, {},  {  824214,  -5743, -452,  238,  -6325,   81 } },
    	{  921600, {},  {  848830,  -3903, -552,  119,  -4030,   -2 } },
    	{  998400, {},  {  901575,  -4409, -584,    0,  -2849,   39 } },
    	{ 1075200, {},  {  940071,  -5367, -602,  -60,    -63,  -93 } },
    	{ 1152000, {},  {  996765,  -6637, -614, -179,   1905,  -13 } },
    	{ 1228800, {},  { 1098475, -13529, -497, -179,   3626,    9 } },
    	{ 1267200, {},  { 1131060, -13109, -573,  -90,   2352,   25 } },
    	{ 1305600, {},  { 1163644, -12688, -648,    0,   1077,   40 } },
},

//.eristaMtcTable = const_cast<EristaMtcTable *>(&EristaMtcTablePlaceholder),
//.marikoMtcTable = const_cast<MarikoMtcTable *>(&MarikoMtcTablePlaceholder),

};

}