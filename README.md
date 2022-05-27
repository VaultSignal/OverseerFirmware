# VaultSignal - Overseer

This is the firmware for the Overseer device, used by the VaultSignal project to receive data from Dwellers using
radio transmissions and transmit it to a central server on the internet using websockets.

## Initial Installation

You must first install the ESP-IDF Framework before anything, ESPRESSIF has the documentation to do so in their website,
then, initialize git submodules to fetch the components used by the Overseer.

```
git submodule update --init --recursive
idf.py menuconfig
```

After this, you can use the ESP-IDF Toolchain to compile, link and flash your device.
