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
#ifndef RADIO_RECEIVER_HPP
#define RADIO_RECEIVER_HPP
#include <memory>
#include "NetworkClient.hpp"
#include "DeviceEvent.hpp"
#include "RF24.h"

/* How to determine sender and receiver */
#define NRF_MODE_SENDER 1
#define NRF_MODE_RECEIVER 2

namespace VaultSignal
{
    class RadioReceiver
    {
    private:
        RF24 *receiver;
        int connectionsReceived;
        std::unique_ptr<DeviceEvent> encodeDeviceEvent(uint8_t *payload);
        constexpr static const uint64_t deviceAddress = 0;
        /**
         * @brief Print the payload to the logger.
         *
         * @param payload Payload to print.
         * @param payloadSize Size of the payload in bytes.
         */
        void logPayload(uint8_t *payload, int payloadSize);

    public:
        /**
         * @brief Receive a group of messages and send them to the network client.
         *
         * @param client Client to send the messages to.
         */
        void receiveMessages(NetworkClient &client);
        RadioReceiver();
        ~RadioReceiver();
    };
};

#endif // RADIO_RECEIVER_HPP
