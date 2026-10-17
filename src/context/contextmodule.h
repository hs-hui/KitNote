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
#ifndef KN_CONTEXT_CONTEXTMODULE_H
#define KN_CONTEXT_CONTEXTMODULE_H

#include <memory>

#include "modularity/imodulesetup.h"

namespace kn::context {
class GlobalContext;
class UiContextResolver;
class ContextModule : public kfw::modularity::IModuleSetup
{
public:

    std::string moduleName() const override;
    void registerExports() override;
    void onInit(const kfw::IApplication::RunMode& mode) override;
    void onDeinit() override;

private:
    std::shared_ptr<GlobalContext> m_globalContext;
    std::shared_ptr<UiContextResolver> m_uicontextResolver;
};
}

#endif // KN_CONTEXT_CONTEXTMODULE_H
