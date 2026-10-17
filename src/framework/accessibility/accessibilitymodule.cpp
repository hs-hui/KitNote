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
#include "accessibilitymodule.h"

#include "modularity/ioc.h"

#include "internal/accessibilitycontroller.h"
#include "internal/accessibilityconfiguration.h"
#include "internal/qaccessibleinterfaceregister.h"


using namespace kfw::accessibility;
using namespace kfw::modularity;

std::string AccessibilityModule::moduleName() const
{
    return "accessibility";
}

void AccessibilityModule::registerExports()
{
    m_configuration = std::make_shared<AccessibilityConfiguration>(iocContext());
    m_controller = std::make_shared<AccessibilityController>(iocContext());

    ioc()->registerExport<IAccessibilityConfiguration>(moduleName(), m_configuration);
    ioc()->registerExport<IAccessibilityController>(moduleName(), m_controller);
    ioc()->registerExport<IQAccessibleInterfaceRegister>(moduleName(), new QAccessibleInterfaceRegister());
}

void AccessibilityModule::resolveImports()
{
    auto accr = ioc()->resolve<IQAccessibleInterfaceRegister>(moduleName());
    if (accr) {
#ifndef Q_OS_LINUX // https://github.com/musescore/MuseScore/pull/32258#issuecomment-3972545361
        accr->registerInterfaceGetter("QQuickWindow", AccessibilityController::accessibleInterface);
#endif
        accr->registerInterfaceGetter("kfw::accessibility::AccessibleObject", AccessibleObject::accessibleInterface);
    }
}

void AccessibilityModule::onPreInit(const IApplication::RunMode&)
{
    m_controller->setAccesibilityEnabled(true);
}

void AccessibilityModule::onInit(const IApplication::RunMode&)
{
    m_configuration->init();
}
