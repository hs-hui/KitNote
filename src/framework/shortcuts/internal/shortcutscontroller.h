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
#ifndef KFW_SHORTCUTS_SHORTCUTSCONTROLLER_H
#define KFW_SHORTCUTS_SHORTCUTSCONTROLLER_H

#include "../ishortcutscontroller.h"

#include "async/asyncable.h"
#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"
#include "ui/iuiactionsregister.h"
#include "ui/iinteractiveprovider.h"
#include "ui/iuicontextresolver.h"
#include "ishortcutsregister.h"
#include "shortcutcontext.h"

namespace kfw::shortcuts {
class ShortcutsController : public IShortcutsController, public Contextable, public async::Asyncable
{
    ContextInject<kfw::ui::IUiActionsRegister> aregister = { this };
    ContextInject<IShortcutsRegister> shortcutsRegister = { this };
    ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    ContextInject<kfw::ui::IInteractiveProvider> interactiveProvider = { this };
    ContextInject<kfw::ui::IUiContextResolver> uiContextResolver = { this };

    //! NOTE May be missing because it must be implemented outside the framework
    ContextInject<IShortcutContextPriority> shortcutContextPriority = { this };

public:
    ShortcutsController(const modularity::ContextPtr& iocCtx)
        : Contextable(iocCtx) {}

    void init();

    void activate(const std::string& sequence) override;
    bool isRegistered(const std::string& sequence) const override;

private:
    kfw::actions::ActionCode resolveAction(const std::string& sequence) const;
};
}

#endif // KFW_SHORTCUTS_SHORTCUTSCONTROLLER_H
