/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
 * Modified for KitNote, 2026
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <gmock/gmock.h>

#include "network/inetworkmanager.h"

namespace kfw::network {
class NetworkManagerMock : public INetworkManager
{
public:
    MOCK_METHOD(RetVal<Progress>, get, (const QUrl&, IncomingDevicePtr, const RequestHeaders&), (override));
    MOCK_METHOD(RetVal<Progress>, head, (const QUrl&, const RequestHeaders&), (override));
    MOCK_METHOD(RetVal<Progress>, post, (const QUrl&, OutgoingDeviceVar, IncomingDevicePtr, const RequestHeaders&), (override));
    MOCK_METHOD(RetVal<Progress>, put, (const QUrl&, OutgoingDeviceVar, IncomingDevicePtr, const RequestHeaders&), (override));
    MOCK_METHOD(RetVal<Progress>, patch, (const QUrl&, OutgoingDeviceVar, IncomingDevicePtr, const RequestHeaders&), (override));
    MOCK_METHOD(RetVal<Progress>, del, (const QUrl&, IncomingDevicePtr, const RequestHeaders&), (override));
};
}
