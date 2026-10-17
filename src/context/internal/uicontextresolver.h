/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include "../iuicontextresolver.h"
#include "async/asyncable.h"
#include "modularity/ioc.h"
#include "iinteractive.h"
#include "../iglobalcontext.h"
#include "ui/inavigationcontroller.h"

namespace kn::context {
class UiContextResolver : public IUiContextResolver, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::ui::INavigationController> navigationController = { this };

public:
    UiContextResolver(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void init();

    const kfw::ui::UiContext& currentUiContext() const override;
    kfw::async::Notification currentUiContextChanged() const override;

    bool match(const kfw::ui::UiContext& currentCtx, const kfw::ui::UiContext& actCtx) const override;
    bool matchWithCurrent(const kfw::ui::UiContext& ctx) const override;

    bool isShortcutContextAllowed(const std::string& scContext) const override;

private:
    void updateCurrentUiContext();
    kfw::ui::UiContext resolveCurrentUiContext() const;

    kfw::ui::UiContext m_currentUiContext;
    kfw::async::Notification m_currentUiContextChanged;
};
}
