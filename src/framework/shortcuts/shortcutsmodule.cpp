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
#include "shortcutsmodule.h"

#include "modularity/ioc.h"

#include "internal/shortcutsregister.h"
#include "internal/shortcutscontroller.h"
#include "internal/shortcutsconfiguration.h"


#include "kfw_framework_config.h"

#ifdef KFW_MODULE_DIAGNOSTICS
#include "diagnostics/idiagnosticspathsregister.h"
#endif

using namespace kfw::shortcuts;
using namespace kfw::modularity;
using namespace kfw::ui;

std::string ShortcutsModule::moduleName() const
{
    return "shortcuts";
}

void ShortcutsModule::registerExports()
{
    m_shortcutsController = std::make_shared<ShortcutsController>(iocContext());
    m_shortcutsRegister = std::make_shared<ShortcutsRegister>(iocContext());
    m_configuration = std::make_shared<ShortcutsConfiguration>(iocContext());

    ioc()->registerExport<IShortcutsRegister>(moduleName(), m_shortcutsRegister);
    ioc()->registerExport<IShortcutsController>(moduleName(), m_shortcutsController);
    ioc()->registerExport<IShortcutsConfiguration>(moduleName(), m_configuration);
}

void ShortcutsModule::onInit(const IApplication::RunMode&)
{
    m_configuration->init();
    m_shortcutsController->init();

#ifdef KFW_MODULE_DIAGNOSTICS
    auto pr = ioc()->resolve<kfw::diagnostics::IDiagnosticsPathsRegister>(moduleName());
    if (pr) {
        pr->reg("shortcutsUserAppDataPath", m_configuration->shortcutsUserAppDataPath());
        pr->reg("shortcutsAppDataPath", m_configuration->shortcutsAppDataPath());
    }
#endif
}

void ShortcutsModule::onAllInited(const IApplication::RunMode&)
{
    m_shortcutsRegister->init();
}
