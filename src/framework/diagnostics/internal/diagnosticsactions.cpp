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
#include "diagnosticsactions.h"

#include "ui/uiaction.h"
#include "shortcuts/shortcutcontext.h"
#include "types/translatablestring.h"

using namespace kfw;
using namespace kfw::ui;
using namespace kfw::actions;
using namespace kfw::diagnostics;

const UiActionList DiagnosticsActions::m_actions = {
    UiAction("diagnostic-save-diagnostic-files",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "Save diagnostic files")
             ),
    UiAction("diagnostic-show-paths",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "Show p&aths…"),
             TranslatableString("action", "Show paths")
             ),
    UiAction("diagnostic-show-profiler",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "Show pr&ofiler…"),
             TranslatableString("action", "Show profiler")
             ),
    UiAction("diagnostic-show-graphicsinfo",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "Show g&raphics info…"),
             TranslatableString("action", "Show graphics info")
             ),
    UiAction("diagnostic-show-navigation-tree",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "Show &navigation tree…"),
             TranslatableString("action", "Show navigation tree")
             ),
    UiAction("diagnostic-show-accessible-tree",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "Show &accessibility tree…"),
             TranslatableString("action", "Show accessibility tree")
             ),
    UiAction("diagnostic-accessible-tree-dump",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "&Dump accessibility tree to console"),
             TranslatableString("action", "Dump accessibility tree to console")
             ),
    UiAction("diagnostic-show-engraving-elements",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "Show engraving &elements"),
             TranslatableString("action", "Show engraving elements")
             ),
    UiAction("diagnostic-show-engraving-undostack",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "Show engraving &undo stack"),
             TranslatableString("action", "Show engraving undo stack")
             ),
    UiAction("diagnostic-show-engraving-style",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "Show engraving &style options list"),
             TranslatableString("action", "Show engraving style options list")
             ),
    UiAction("diagnostic-show-actions",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString("action", "Show &actions list"),
             TranslatableString("action", "Show actions list")
             ),
    UiAction("action://diagnostic/actions/query",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString::untranslatable("Test query action")
             ),
    UiAction("action://diagnostic/actions/query_params1",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString::untranslatable("Test query action with params 1")
             ),
    UiAction("action://diagnostic/actions/query_params2",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString::untranslatable("Test query action with params 2")
             )
};

const UiActionList& DiagnosticsActions::actionsList() const
{
    return m_actions;
}

bool DiagnosticsActions::actionEnabled(const UiAction&) const
{
    return true;
}

kfw::async::Channel<ActionCodeList> DiagnosticsActions::actionEnabledChanged() const
{
    static kfw::async::Channel<ActionCodeList> ch;
    return ch;
}

bool DiagnosticsActions::actionChecked(const UiAction&) const
{
    return false;
}

kfw::async::Channel<ActionCodeList> DiagnosticsActions::actionCheckedChanged() const
{
    static kfw::async::Channel<ActionCodeList> ch;
    return ch;
}
