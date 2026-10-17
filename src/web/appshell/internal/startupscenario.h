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

#include "istartupscenario.h"

#include "global/modularity/ioc.h"
#include "global/iinteractive.h"

namespace kn::appshell {
class StartupScenario : public IStartupScenario, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:
    StartupScenario(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void setStartupType(const std::optional<std::string>& type) override;

    bool isStartWithNewFileAsSecondaryInstance() const override;

    const project::ProjectFile& startupScoreFile() const override;
    void setStartupScoreFile(const std::optional<project::ProjectFile>& file) override;

    kfw::async::Promise<kfw::Ret> runOnSplashScreen() override;
    void runAfterSplashScreen() override;
    bool startupCompleted() const override;

private:

    bool m_startupCompleted = false;
};
}
