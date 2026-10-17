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
#include "projectuiactions.h"

#include "modularity/ioc.h"
#include "types/translatablestring.h"
#include "context/shortcutcontext.h"

using namespace kn::project;
using namespace kfw;
using namespace kfw::ui;
using namespace kfw::actions;

const UiActionList ProjectUiActions::m_actions = {
    UiAction("file-open",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Open…"),
             TranslatableString("action", "Open"),
             IconCode::Code::OPEN_FILE
             ),
    UiAction("file-new",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&New…"),
             TranslatableString("action", "New"),
             IconCode::Code::NEW_FILE
             ),
    UiAction("file-close",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Close"),
             TranslatableString("action", "Close")
             ),
    UiAction("file-save",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Save"),
             TranslatableString("action", "Save"),
             IconCode::Code::SAVE
             ),
    UiAction("file-save-as",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Save &as…"),
             TranslatableString("action", "Save as")
             ),
    UiAction("file-save-a-copy",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Save a &copy…"),
             TranslatableString("action", "Save a copy")
             ),
    UiAction("file-save-selection",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Save &selection…"),
             TranslatableString("action", "Save selection")
             ),
    UiAction("file-export",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Export…"),
             TranslatableString("action", "Export"),
             IconCode::Code::SHARE_FILE
             ),
    UiAction("project-properties",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Project propert&ies…"),
             TranslatableString("action", "Project properties")
             ),
    UiAction("print",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Print…"),
             TranslatableString("action", "Print"),
             IconCode::Code::PRINT
             ),
    UiAction("clear-recent",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Clear list of recent files"),
             TranslatableString("action", "Clear list of recent files")
             )
};

ProjectUiActions::ProjectUiActions(std::shared_ptr<ProjectActionsController> controller, const kfw::modularity::ContextPtr& iocCtx)
    : kfw::Contextable(iocCtx), m_controller(controller)
{
}

const UiActionList& ProjectUiActions::actionsList() const
{
    return m_actions;
}

bool ProjectUiActions::actionEnabled(const UiAction& act) const
{
    if (!m_controller->canReceiveAction(act.code)) {
        return false;
    }

    return true;
}

bool ProjectUiActions::actionChecked(const UiAction&) const
{
    return false;
}

kfw::async::Channel<ActionCodeList> ProjectUiActions::actionEnabledChanged() const
{
    return m_actionEnabledChanged;
}

kfw::async::Channel<ActionCodeList> ProjectUiActions::actionCheckedChanged() const
{
    return m_actionCheckedChanged;
}
