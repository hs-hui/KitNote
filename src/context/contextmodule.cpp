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
#include "contextmodule.h"

#include "modularity/ioc.h"
#include "internal/globalcontext.h"
#include "internal/uicontextresolver.h"
#include "shortcutcontext.h"

using namespace kn::context;
using namespace kfw::modularity;
using namespace kfw::shortcuts;

std::string ContextModule::moduleName() const
{
    return "context";
}

void ContextModule::registerExports()
{
    m_globalContext = std::make_shared<GlobalContext>();
    m_uicontextResolver = std::make_shared<UiContextResolver>(iocContext());

    ioc()->registerExport<IGlobalContext>(moduleName(), m_globalContext);
    ioc()->registerExport<IUiContextResolver>(moduleName(), m_uicontextResolver);
    ioc()->registerExport<IShortcutContextPriority>(moduleName(), new ShortcutContextPriority());
}

void ContextModule::onInit(const kfw::IApplication::RunMode& mode)
{
    if (mode != kfw::IApplication::RunMode::GuiApp) {
        return;
    }

    m_uicontextResolver->init();
}

void ContextModule::onDeinit()
{
    m_globalContext->setCurrentProject(nullptr);
}
