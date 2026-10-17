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

#include "paletteuiactions.h"

#include "context/uicontext.h"
#include "context/shortcutcontext.h"
#include "modularity/ioc.h"
#include "types/translatablestring.h"

using namespace kn::palette;
using namespace kfw;
using namespace kfw::ui;
using namespace kfw::actions;

static const kfw::actions::ActionCode MASTERPALETTE_CODE("masterpalette");

const UiActionList PaletteUiActions::m_actions = {
    UiAction(MASTERPALETTE_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Master palette…"),
             TranslatableString("action", "Open master palette"),
             Checkable::Yes
             ),
    UiAction("palette-search",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "Palette search"),
             TranslatableString("action", "Search palettes")
             ),
    UiAction("time-signature-properties",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Time signature properties…"),
             TranslatableString("action", "Time signature properties")
             ),
    UiAction("customize-kit",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Customize kit…"),
             TranslatableString("action", "Customize kit")
             ),
    UiAction("apply-current-palette-element",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Apply current palette element"),
             TranslatableString("action", "Apply current palette element")
             ),
    UiAction("show-keys",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Insert special characters…"),
             TranslatableString("action", "Insert special characters")
             )
};

PaletteUiActions::PaletteUiActions(std::shared_ptr<PaletteActionsController> controller, const kfw::modularity::ContextPtr& iocCtx)
    : kfw::Contextable(iocCtx), m_controller(controller)
{
}

void PaletteUiActions::init()
{
    m_controller->isMasterPaletteOpened().ch.onReceive(this, [this](bool) {
        m_actionCheckedChanged.send({ MASTERPALETTE_CODE });
    });
}

const UiActionList& PaletteUiActions::actionsList() const
{
    return m_actions;
}

bool PaletteUiActions::actionEnabled(const UiAction& act) const
{
    if (!m_controller->canReceiveAction(act.code)) {
        return false;
    }

    return true;
}

bool PaletteUiActions::actionChecked(const UiAction& act) const
{
    if (MASTERPALETTE_CODE == act.code) {
        return m_controller->isMasterPaletteOpened().val;
    }

    return false;
}

kfw::async::Channel<ActionCodeList> PaletteUiActions::actionEnabledChanged() const
{
    return m_actionEnabledChanged;
}

kfw::async::Channel<ActionCodeList> PaletteUiActions::actionCheckedChanged() const
{
    return m_actionCheckedChanged;
}
