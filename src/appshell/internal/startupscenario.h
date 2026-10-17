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

#pragma once

#include "istartupscenario.h"

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "iinteractive.h"
#include "actions/iactionsdispatcher.h"
#include "multiwindows/imultiwindowsprovider.h"
#include "iappshellconfiguration.h"
#include "isessionsmanager.h"
#include "project/iprojectautosaver.h"
#include "audioplugins/iregisteraudiopluginsscenario.h"

#include "framework/stubs/update/iappupdatescenario.h"

namespace kn::appshell {
class StartupScenario : public IStartupScenario, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<kfw::mi::IMultiWindowsProvider> multiwindowsProvider;
    kfw::GlobalInject<IAppShellConfiguration> configuration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<ISessionsManager> sessionsManager = { this };
    kfw::ContextInject<project::IProjectAutoSaver> projectAutoSaver = { this };
    kfw::ContextInject<kfw::audioplugins::IRegisterAudioPluginsScenario> registerAudioPluginsScenario = { this };
    kfw::ContextInject<kfw::update::IAppUpdateScenario> appUpdateScenario = { this };

public:
    StartupScenario(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void setStartupType(const std::optional<std::string>& type) override;

    bool isStartWithNewFileAsSecondaryInstance() const override;

    const project::ProjectFile& startupScoreFile() const override;
    void setStartupScoreFile(const std::optional<project::ProjectFile>& file) override;

    void runOnSplashScreen() override;
    void runAfterSplashScreen() override;
    bool startupCompleted() const override;

private:
    void registerAudioPlugins();

    StartupModeType resolveStartupModeType() const;

    void onStartupPageOpened(StartupModeType modeType);

    void showStartupDialogsIfNeed(StartupModeType modeType);
    bool shouldShowWelcomeDialog(StartupModeType modeType) const;

    void openScore(const project::ProjectFile& file);

    void restoreLastSession();
    void removeProjectsUnsavedChanges(const kfw::io::paths_t& projectsPaths);

    std::string m_startupTypeStr;
    project::ProjectFile m_startupScoreFile;
    bool m_startupCompleted = false;
    size_t m_activeUpdateCheckCount = 0;
};
}
