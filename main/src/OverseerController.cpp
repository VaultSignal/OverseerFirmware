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
#include "OverseerController.hpp"
#include "Arduino.h"
#include "esp_mac.h"
#include "hal/spi_types.h"
#include "driver/spi_common.h"

uint8_t VaultSignal::OverseerController::deviceID = 0;

void VaultSignal::OverseerController::blinkLED(time_t secondsOn, time_t secondsOff, LedPin ledPin)
{
    digitalWrite(ledPin, LOW);
    delay((uint32_t)secondsOn);
    digitalWrite(ledPin, HIGH);
    delay((uint32_t)secondsOff);
}

void VaultSignal::OverseerController::setLEDState(LedPin ledPin, LedState state)
{
    digitalWrite(ledPin, state);
}

void VaultSignal::OverseerController::initialiseWatcher()
{
    Serial.begin(115200);
    ESP_LOGI(SYSTAG, "Initialising the pins.");
    pinMode(LedPin::WIFI_PIN, OUTPUT);
    pinMode(LedPin::WEBSOCKET_PIN, OUTPUT);
    pinMode(LedPin::RADIO_PIN, OUTPUT);
    esp_read_mac(&OverseerController::deviceID, ESP_MAC_WIFI_STA);
    // Initialise the SPI bus.
    // As per https://github.com/ReDFoX43rus/nRF24L01p-ESP32-Library
    ESP_LOGI(SYSTAG, "Initialising the SPI.");
    spi_bus_config_t buscfg;
    memset(&buscfg, 0, sizeof(buscfg));
    buscfg.miso_io_num = RF_SPI_MISO;
    buscfg.mosi_io_num = RF_SPI_MOSI;
    buscfg.sclk_io_num = RF_SPI_SCLK;
    buscfg.quadhd_io_num = -1;
    buscfg.quadwp_io_num = -1;
    buscfg.max_transfer_sz = 4096;
    esp_err_t err = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    assert(err == ESP_OK);
    ESP_LOGI(SYSTAG, "Device init complete.");
}

void VaultSignal::OverseerController::setAllLEDs(VaultSignal::LedState state)
{
    static const std::array<LedPin, 3> ledPins = {LedPin::WIFI_PIN, LedPin::WEBSOCKET_PIN, LedPin::RADIO_PIN};
    for (const auto &pin : ledPins)
    {
        setLEDState(pin, state);
    }
}

void VaultSignal::OverseerController::unlitAll()
{
    setAllLEDs(LedState::OFF);
}

void VaultSignal::OverseerController::litAll()
{
    setAllLEDs(LedState::ON);
}