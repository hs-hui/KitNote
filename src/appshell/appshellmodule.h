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

#include <memory>

#include "modularity/imodulesetup.h"

namespace kn::appshell {
class ApplicationActionController;
class ApplicationUiActions;
class AppShellConfiguration;
class SessionsManager;
class AppShellModule : public kfw::modularity::IModuleSetup
{
public:

    std::string moduleName() const override;

    void registerExports() override;
    void resolveImports() override;

    void onPreInit(const kfw::IApplication::RunMode& mode) override;
    void onInit(const kfw::IApplication::RunMode& mode) override;
    void onAllInited(const kfw::IApplication::RunMode& mode) override;
    void onDeinit() override;

    // Context
    kfw::modularity::IContextSetup* newContext(const kfw::modularity::ContextPtr& ctx) const override;

private:
    std::shared_ptr<AppShellConfiguration> m_appShellConfiguration;
    std::shared_ptr<SessionsManager> m_sessionsManager;
};

class AppShellContext : public kfw::modularity::IContextSetup
{
public:
    AppShellContext(const kfw::modularity::ContextPtr& ctx)
        : kfw::modularity::IContextSetup(ctx) {}

    void registerExports() override;
    void resolveImports() override;
    void onPreInit(const kfw::IApplication::RunMode& mode) override;
    void onInit(const kfw::IApplication::RunMode& mode) override;

private:
    std::shared_ptr<ApplicationActionController> m_applicationActionController;
    std::shared_ptr<ApplicationUiActions> m_applicationUiActions;
};
}
