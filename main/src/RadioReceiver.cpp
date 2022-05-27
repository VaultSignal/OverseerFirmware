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
#include "RadioReceiver.hpp"
#include "WatcherController.hpp"
#include "freertos/FreeRTOS.h"
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include "freertos/task.h"

void VaultSignal::RadioReceiver::receiveMessages(VaultSignal::NetworkClient &client)
{
    this->receiver->begin();
    ESP_LOGI(RFTAG, "Message loop started.");
    WatcherController::blinkLED(100, 100, LedPin::RADIO_PIN);
    WatcherController::setLEDState(LedPin::RADIO_PIN, LedState::OFF);
    uint8_t buffer[28] = {0};
    this->receiver->openReadingPipe(0, this->deviceAddress);
    this->receiver->setPALevel(RF24_PA_MAX);
    this->receiver->startListening();
    while (true)
    {
        if (this->receiver->available())
        {
            ESP_LOGI(RFTAG, "Message Received");
            WatcherController::setLEDState(LedPin::RADIO_PIN, LedState::ON);
            this->receiver->read(buffer, 28);
            logPayload(buffer, 28);
            uint8_t *payload = (uint8_t *)malloc(sizeof(uint8_t) * 28);
            payload = (uint8_t *)std::memcpy(payload, buffer, 28);
            auto event = std::unique_ptr<DeviceEvent>{reinterpret_cast<DeviceEvent *>(payload)};
            client.queueForUpload(std::move(event));
            WatcherController::setLEDState(LedPin::RADIO_PIN, LedState::OFF);
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

VaultSignal::RadioReceiver::RadioReceiver()
{
    ESP_LOGI(RFTAG, "Initialising the radio receiver.");
    this->receiver = new RF24(4, 5);
    ESP_LOGI(RFTAG, "Radio receiver initialised.");
}

VaultSignal::RadioReceiver::~RadioReceiver()
{
    delete this->receiver;
    this->connectionsReceived = 0;
}

void VaultSignal::RadioReceiver::logPayload(uint8_t *payload, int payloadSize)
{
    std::string log;
    char ch[3];
    for (int i = 0; i < payloadSize; i++)
    {
        snprintf(ch, 3, "%02X", (int)payload[i]);
        log += ch;
    }
    const char *cstr = log.c_str();
    ESP_LOGI(RFTAG, "%s", cstr);
}
