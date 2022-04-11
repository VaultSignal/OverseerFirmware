#! /bin/bash
mkdir components
mkdir temp
git clone https://github.com/nopnop2002/esp-idf-mirf temp/esp-idf-mirf
cp -r temp/esp-idf-mirf/components/mirf components/mirf
rm -rf temp
mkdir -p components
cd components
git clone https://github.com/espressif/arduino-esp32.git arduino
cd arduino
git submodule update --init --recursive
cd ../..
idf.py menuconfig
