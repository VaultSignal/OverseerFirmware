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
#ifndef DEVICE_PAYLOAD_HPP
#define DEVICE_PAYLOAD_HPP

namespace VaultSignal
{
    /**
     * @brief Represents an event emitted from a device in the
     * fleet.
     */
    struct DeviceEvent
    {
        uint8_t deviceID;
        uint8_t hasOpened;
        uint8_t hasMoved;
        uint8_t hasLight;
        float xAccel;
        float yAccel;
        float zAccel;
        int16_t padding1;
        int16_t ldrOne;
        int16_t padding2;
        int16_t ldrTwo;
        int16_t padding3;
        int16_t ldrThree;
    };

#define EXPECTED_DEVICE_EVENT_SIZE (28 * sizeof(uint8_t))
    static_assert(sizeof(DeviceEvent) == EXPECTED_DEVICE_EVENT_SIZE, "Using padding!");

};

#endif // DEVICE_PAYLOAD_HPP