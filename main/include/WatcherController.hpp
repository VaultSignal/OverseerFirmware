/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * VaultSignal Overseer Firmware - Firmware for Overseer
 * Copyright (C) 2022  VaultSignal
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef WATCHER_CONTROLLER_HPP
#define WATCHER_CONTROLLER_HPP
#include "Arduino.h"
#include <time.h>
#include <array>

#define RF_SPI_SCLK 18
#define RF_SPI_MISO 19
#define RF_SPI_MOSI 23

namespace VaultSignal
{
    constexpr const char *TAG = "overseer_network";
    constexpr const char *RFTAG = "overseer_radio";
    constexpr const char *APTAG = "overseer_softap";
    constexpr const char *SYSTAG = "overseer_system";

    typedef enum LedPin : uint8_t
    {
        WIFI_PIN = 33,
        WEBSOCKET_PIN = 32,
        RADIO_PIN = 25
    } LedPin;

    typedef enum LedState : uint8_t
    {
        ON = HIGH,
        OFF = LOW
    } LedState;

    class WatcherController
    {
    private:
        /**
         * @brief Set all LED states.
         *
         */
        static void setAllLEDs(LedState state);

    public:
        /**
         * @brief Blink the built-in led of the device once.
         *
         * @param seconds_on Seconds it will stay lit.
         * @param seconds_off seconds it will stay unlit.
         */
        static void blinkLED(time_t secondsOn, time_t secondsOff, LedPin ledPin);
        /**
         * @brief Set the state of th LED.
         *
         * @param ledPin Pin connected to the LED.
         * @param state State of the LED to set.
         */
        static void setLEDState(LedPin ledPin, LedState state);
        /**
         * @brief Initialise the watcher device.
         *
         * Initialise the watcher device's state.
         */
        static void initialiseWatcher(void);
        /**
         * @brief Lit all status LEDs.
         *
         */
        static void litAll(void);
        /**
         * @brief Unlit all status LEDs.
         *
         */
        static void unlitAll(void);
        static uint8_t deviceID;
    };

};

#endif // WATCHER_CONTROLLER_HPP