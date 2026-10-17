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
#include "appupdatescenariostub.h"

using namespace kfw::update;

bool AppUpdateScenarioStub::needCheckForUpdate() const
{
    return false;
}

void AppUpdateScenarioStub::checkForUpdate(bool)
{
}

bool AppUpdateScenarioStub::checkInProgress() const
{
    return false;
}

kfw::async::Notification AppUpdateScenarioStub::checkInProgressChanged() const
{
    return {};
}

bool AppUpdateScenarioStub::hasUpdate() const
{
    return false;
}

kfw::async::Promise<kfw::Ret> AppUpdateScenarioStub::showUpdate()
{
    return kfw::async::Promise<kfw::Ret>([](auto /*resolve*/, auto reject) {
        return reject(int(kfw::Ret::Code::UnknownError), "stub");
    });
}
