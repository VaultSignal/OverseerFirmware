# VaultSignal - Overseer

![VaultSignal Logo depicting a padlock with a wifi signal sign in the middle and PCB traces moving radiating outwards from the lock.](resources/noun-secure-connection-4316643.svg|width=50)

This is the firmware for the Overseer device, used by the VaultSignal project to receive data from Dwellers using
radio transmissions and transmit it to a central server on the internet using websockets.

## Initial Installation

You must first install the ESP-IDF Framework before anything, ESPRESSIF has
[the documentation to do so in their website](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/get-started/index.html#installation-step-by-step),
then, initialize git submodules to fetch the components used by the Overseer.

```
git submodule update --init --recursive
idf.py menuconfig
```

After this, you can use the ESP-IDF Toolchain to compile, link and flash your device.

## Normal Working

1. When the device is initally booted, it will open a WiFi Access point titled "VaultSignal Overseer"
2. When the user connects to this WiFi Access Point, a login page will pop up.
3. The login page will list all other WiFi access points the Overseer can connect to, the user must choose one and provide its password.
4. Overseer will disconnect the access point, and connect
   to the access point provided by the user.
5. Overseer will wait for new packets from Dwellers using
   its built in radio receiver and transmit these events to
   the central websocket server.

## LED Descriptions

State of the LEDs on the device can be used to debug
any problems that may arise.

| Green Led | Blue LED | Red LED  | Explanation          |
| --------- | -------- | -------- | -------------------- |
| Lit       | Lit      | Lit      | Awaiting credentials |
| Unlit     | Blinking | Any      | Connecting to WiFi   |
| Blinking  | Lit      | Any      | Connecting to server |
| Any       | Any      | Blinking | Radio message on lit |

## Schematic

Fritzing file for the schematic [can be found under resources](resources/Overseer%20Schematic.fzz).

![Schematic for the Overseer showing each component and their connections.](resources/Overseer%20Schematic_schem.png)
