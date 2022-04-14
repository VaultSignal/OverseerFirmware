#! /bin/bash
mkdir components
git clone https://github.com/ReDFoX43rus/nRF24L01p-ESP32-Library components/NRF024L01p
cd components
git clone https://github.com/espressif/arduino-esp32.git arduino
cd arduino
git submodule update --init --recursive
cd ../..
idf.py menuconfig
