/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2026 MuseScore Limited and others
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

#include "../../imultiwindowsprovider.h"

#include "global/modularity/ioc.h"
#include "global/iapplication.h"

//! NOTE Work in progress

namespace kfw::ui {
class IMainWindow;
}

namespace kfw::actions {
class IActionsDispatcher;
}

namespace kfw::mi {
class IProjectProvider;
class SingleProcessProvider : public IMultiWindowsProvider
{
    ContextInject<IApplication> application = { nullptr };

public:

    void init() {}

    // Windows info
    int windowCount() const override;

    // Project opening
    bool isProjectAlreadyOpened(const kfw::io::path_t& projectPath) const override;
    void activateWindowWithProject(const kfw::io::path_t& projectPath) override;
    bool isHasWindowWithoutProject() const override;
    void activateWindowWithoutProject(const QStringList& args = { }) override;
    bool openNewWindow(const QStringList& args) override;

    // Settings
    bool isPreferencesAlreadyOpened() const override { return false; }
    void activateWindowWithOpenedPreferences() const override {}
    void settingsBeginTransaction() override {}
    void settingsCommitTransaction() override {}
    void settingsRollbackTransaction() override {}
    void settingsReset() override {}
    void settingsSetValue(const std::string&, const kfw::Val&) override {}

    // Resources (files)
    bool lockResource(const std::string&) override { return false; }
    bool unlockResource(const std::string&) override { return false; }
    void notifyAboutResourceChanged(const std::string&) override { }
    kfw::async::Channel<std::string> resourceChanged() override { return {}; }

    // Quit for all
    void notifyAboutWindowWasQuited() override {}
    void quitForAll() override {}
    void quitAllAndRestartLast() override {}
    void quitAllAndRunInstallation(const kfw::io::path_t&) override {}

private:
    std::shared_ptr<IProjectProvider> projectProvider(const modularity::ContextPtr& ctx) const;
    std::shared_ptr<ui::IMainWindow> mainWindow(const modularity::ContextPtr& ctx) const;
    std::shared_ptr<actions::IActionsDispatcher> dispatcher(const modularity::ContextPtr& ctx) const;
};
}
