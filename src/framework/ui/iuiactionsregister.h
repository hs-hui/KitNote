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
#ifndef KFW_UI_IUIACTIONSREGISTER_H
#define KFW_UI_IUIACTIONSREGISTER_H

#include "modularity/imoduleinterface.h"
#include "global/async/channel.h"

#include "iuiactionsmodule.h"
#include "uiaction.h"

namespace kfw::ui {
class IUiActionsRegister : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IUiActionsRegister)

public:
    virtual ~IUiActionsRegister() = default;

    virtual void reg(const IUiActionsModulePtr& module) = 0;
    virtual void unreg(const IUiActionsModulePtr& module) = 0;

    virtual std::vector<UiAction> actionList() const = 0;

    virtual const UiAction& action(const actions::ActionCode& code) const = 0;
    virtual const actions::ActionCode& parentActionCode(const actions::ActionCode& code) const = 0;
    virtual async::Channel<UiActionList> actionsChanged() const = 0;

    virtual UiActionState actionState(const actions::ActionCode& code) const = 0;
    virtual async::Channel<actions::ActionCodeList> actionStateChanged() const = 0;
};
}

#endif // KFW_UI_IUIACTIONSREGISTER_H
