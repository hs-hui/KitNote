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
#include "workspaceuiactions.h"

#include "ui/uiaction.h"
#include "shortcuts/shortcutcontext.h"
#include "types/translatablestring.h"

using namespace kfw;
using namespace kfw::workspace;
using namespace kfw::ui;
using namespace kfw::actions;

const UiActionList WorkspaceUiActions::m_actions = {
    UiAction("select-workspace",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_ANY,
             TranslatableString("action", "Select workspace")
             ),
    UiAction("configure-workspaces",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_ANY,
             TranslatableString("action", "Edit workspaces…"),
             TranslatableString("action", "Edit workspaces"),
             IconCode::Code::EDIT
             ),
    UiAction("create-workspace",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_ANY,
             TranslatableString("action", "Create new workspace"),
             IconCode::Code::PLUS
             ),
};

WorkspaceUiActions::WorkspaceUiActions(std::shared_ptr<WorkspaceActionController> controller, const modularity::ContextPtr& iocCtx)
    : Contextable(iocCtx), m_controller(controller)
{
}

const UiActionList& WorkspaceUiActions::actionsList() const
{
    return m_actions;
}

bool WorkspaceUiActions::actionEnabled(const UiAction& act) const
{
    if (!m_controller->canReceiveAction(act.code)) {
        return false;
    }

    return true;
}

bool WorkspaceUiActions::actionChecked(const UiAction&) const
{
    return false;
}

async::Channel<ActionCodeList> WorkspaceUiActions::actionEnabledChanged() const
{
    return m_actionEnabledChanged;
}

async::Channel<ActionCodeList> WorkspaceUiActions::actionCheckedChanged() const
{
    return m_actionCheckedChanged;
}
