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
#ifndef KN_APPSHELL_SESSIONSMANAGER_H
#define KN_APPSHELL_SESSIONSMANAGER_H

#include "istartupscenario.h"

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"
#include "context/iglobalcontext.h"
#include "multiwindows/imultiwindowsprovider.h"
#include "project/iprojectconfiguration.h"
#include "iappshellconfiguration.h"

#include "isessionsmanager.h"

namespace kn::appshell {
class SessionsManager : public ISessionsManager, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<kfw::mi::IMultiWindowsProvider> multiwindowsProvider;
    kfw::GlobalInject<project::IProjectConfiguration> projectConfiguration;
    kfw::GlobalInject<IAppShellConfiguration> configuration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    SessionsManager(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void init();
    void deinit();

    bool hasProjectsForRestore() override;

    void restore() override;
    void reset() override;

private:
    void update();

    void removeProjectFromSession(const kfw::io::path_t& projectPath);
    void addProjectToSession(const kfw::io::path_t& projectPath);

    kfw::io::path_t m_lastOpenedProjectPath;
};
}

#endif // KN_APPSHELL_SESSIONSMANAGER_H
