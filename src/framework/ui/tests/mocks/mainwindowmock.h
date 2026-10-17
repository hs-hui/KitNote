/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2021 MuseScore Limited and others
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
#ifndef KFW_UI_MAINWINDOWMOCK_H
#define KFW_UI_MAINWINDOWMOCK_H

#include <gmock/gmock.h>

#include "framework/ui/imainwindow.h"
#include "async/notification.h"

namespace kfw::ui {
class MainWindowMock : public IMainWindow
{
public:
    MOCK_METHOD(void, init, (MainWindowBridge * bridge), (override));
    MOCK_METHOD(void, deinit, (), (override));

    MOCK_METHOD(QWindow*, qWindow, (), (const, override));

    MOCK_METHOD(void, requestShowOnBack, (), (override));
    MOCK_METHOD(void, requestShowOnFront, (), (override));

    MOCK_METHOD(bool, isFullScreen, (), (const, override));
    MOCK_METHOD(async::Notification, isFullScreenChanged, (), (const, override));
    MOCK_METHOD(void, toggleFullScreen, (), (override));

    MOCK_METHOD(QScreen*, screen, (), (const, override));
};
}

#endif // KFW_UI_MAINWINDOWMOCK_H
