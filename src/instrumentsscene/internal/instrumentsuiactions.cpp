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

#include "instrumentsuiactions.h"

#include "context/uicontext.h"
#include "context/shortcutcontext.h"
#include "types/translatablestring.h"

using namespace kn::instrumentsscene;
using namespace kfw;
using namespace kfw::ui;
using namespace kfw::actions;

const UiActionList InstrumentsUiActions::m_actions = {
    UiAction("instruments",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "Add/remove instruments…"),
             TranslatableString("action", "Add/remove instruments")
             ),
    UiAction("change-instrument",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Select instrument…"),
             TranslatableString("action", "Select instrument")
             )
};

const kfw::ui::UiActionList& InstrumentsUiActions::actionsList() const
{
    return m_actions;
}

bool InstrumentsUiActions::actionEnabled(const UiAction&) const
{
    return true;
}

bool InstrumentsUiActions::actionChecked(const UiAction&) const
{
    return false;
}

kfw::async::Channel<ActionCodeList> InstrumentsUiActions::actionEnabledChanged() const
{
    return m_actionEnabledChanged;
}

kfw::async::Channel<ActionCodeList> InstrumentsUiActions::actionCheckedChanged() const
{
    return m_actionCheckedChanged;
}
