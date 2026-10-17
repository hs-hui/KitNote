/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#ifndef KN_MODULARITY_IMODULESETUP_H
#define KN_MODULARITY_IMODULESETUP_H

#include <string>

#include "ioc.h"
#include "../iapplication.h"

namespace kfw::modularity {
class IContextSetup
{
public:

    IContextSetup(const modularity::ContextPtr& ctx)
        : m_ctx(ctx) {}

    virtual ~IContextSetup() = default;

    const modularity::ContextPtr iocContext() const { return m_ctx; }
    ModulesIoC* ioc() const { return modularity::ioc(iocContext()); }

    virtual void registerExports() {}
    virtual void resolveImports() {}
    virtual void onPreInit(const IApplication::RunMode& mode) { (void)mode; }
    virtual void onInit(const IApplication::RunMode& mode) { (void)mode; }
    virtual void onAllInited(const IApplication::RunMode& mode) { (void)mode; }
    virtual void onDeinit() {}

private:
    ContextPtr m_ctx;
};

class IModuleSetup
{
public:

    virtual ~IModuleSetup() {}

    virtual std::string moduleName() const = 0;

    ModulesIoC* globalIoc() const { return kfw::modularity::globalIoc(); }

    virtual void registerExports() {}
    virtual void resolveImports() {}

    virtual void registerResources() {}
    virtual void registerUiTypes() {}

    virtual void onPreInit(const IApplication::RunMode& mode) { (void)mode; }
    virtual void onInit(const IApplication::RunMode& mode) { (void)mode; }
    virtual void onAllInited(const IApplication::RunMode& mode) { (void)mode; }
    virtual void onDelayedInit() {}
    virtual void onDeinit() {}
    virtual void onDestroy() {}

    virtual void onStartApp() {}

    // Context
    virtual IContextSetup* newContext(const kfw::modularity::ContextPtr& ctx) const { (void)ctx; return nullptr; }

    // to remove
    void setApplication(std::shared_ptr<IApplication> app)
    {
        m_application = app;
    }

    std::shared_ptr<IApplication> application() const { return m_application; }

    const modularity::ContextPtr iocContext() const { return m_application ? m_application->iocContext() : kfw::modularity::globalCtx(); }
    ModulesIoC* ioc() const { return m_application ? m_application->ioc() : kfw::modularity::globalIoc(); }

protected:
    std::shared_ptr<IApplication> m_application;
};
}

#endif // KN_MODULARITY_IMODULESETUP_H
