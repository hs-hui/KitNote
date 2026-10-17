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
#include "startupscenario.h"

#include "log.h"

using namespace kfw;
using namespace kn::appshell;

void StartupScenario::setStartupType(const std::optional<std::string>& /*type*/)
{
    NOT_IMPLEMENTED;
}

bool StartupScenario::isStartWithNewFileAsSecondaryInstance() const
{
    return false;
}

const kn::project::ProjectFile& StartupScenario::startupScoreFile() const
{
    static kn::project::ProjectFile file;
    return file;
}

void StartupScenario::setStartupScoreFile(const std::optional<project::ProjectFile>& /*file*/)
{
    NOT_IMPLEMENTED;
}

kfw::async::Promise<Ret> StartupScenario::runOnSplashScreen()
{
    return async::make_promise<Ret>([](auto resolve) {
        return resolve(kfw::make_ok());
    });
}

void StartupScenario::runAfterSplashScreen()
{
    interactive()->open("kitnote://notation").onResolve(this, [this](const Val&) {
        m_startupCompleted = true;
    });
}

bool StartupScenario::startupCompleted() const
{
    return m_startupCompleted;
}
