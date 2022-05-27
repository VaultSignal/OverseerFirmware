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
#ifndef VAULT_SIGNAL_UTILITY_HPP
#define VAULT_SIGNAL_UTILITY_HPP
#include <string>

namespace VaultSignal
{
    class utility
    {
    public:
        /**
         * @brief Convert a boolean to its string representation.
         *
         * @param val Value to be converted.
         * @return std::string "true" for true, "false" for false.
         */
        static const char *booleanToString(bool val);
    };
}; // VaultSignal

#endif // VAULT_SIGNAL_UTILITY_HPP
