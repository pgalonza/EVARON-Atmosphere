#!/bin/bash

set -e

make clean
make -j
./hactool -t kip emummc.kip --uncompressed emummc_unpacked.kip
python3 tools/kip1converter.py emummc_unpacked.kip emummc.data
cat emummc.caps emummc.data > emummc.kipm
