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

#ifndef KFW_ACCESSIBILITY_ACCESSIBILITYMODULE_H
#define KFW_ACCESSIBILITY_ACCESSIBILITYMODULE_H

#include <memory>

#include "modularity/imodulesetup.h"

namespace kfw::accessibility {
class AccessibilityConfiguration;
class AccessibilityController;
class AccessibilityModule : public modularity::IModuleSetup
{
public:
    std::string moduleName() const override;

    void registerExports() override;
    void resolveImports() override;

    void onPreInit(const IApplication::RunMode& mode) override;
    void onInit(const IApplication::RunMode& mode) override;

private:
    std::shared_ptr<AccessibilityConfiguration> m_configuration;
    std::shared_ptr<AccessibilityController> m_controller;
};
}

#endif // KFW_ACCESSIBILITY_ACCESSIBILITYMODULE_H
