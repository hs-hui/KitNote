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

#ifndef KFW_DOCK_DOCKWINDOWACTIONSCONTROLLER_H
#define KFW_DOCK_DOCKWINDOWACTIONSCONTROLLER_H

#include "actions/actionable.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"
#include "../idockwindowprovider.h"

namespace kfw::dock {
class DockWindowActionsController : public kfw::Contextable, public kfw::actions::Actionable
{
    kfw::ContextInject<IDockWindowProvider> dockWindowProvider = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };

public:
    DockWindowActionsController(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void init();

private:
    void setDockOpen(const kfw::actions::ActionData& args);
    void toggleOpened(const kfw::actions::ActionData& args);
    void toggleFloating(const kfw::actions::ActionData& args);

    void restoreDefaultLayout();

    IDockWindow* window() const;
};
}

#endif // KFW_DOCK_DOCKWINDOWACTIONSCONTROLLER_H
