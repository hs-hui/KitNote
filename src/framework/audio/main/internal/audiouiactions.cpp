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
#include "audiouiactions.h"

#include "ui/uiaction.h"
#include "shortcuts/shortcutcontext.h"
#include "types/translatablestring.h"

#include "audioactionscontroller.h"

using namespace kfw;
using namespace kfw::ui;
using namespace kfw::actions;
using namespace kfw::audio;

const UiActionList AudioUiActions::m_actions = {
    UiAction("action://audio/dev/use-workermode",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString::untranslatable("Worker mode"),
             Checkable::Yes
             ),
    UiAction("action://audio/dev/use-drivermode",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString::untranslatable("Driver mode"),
             Checkable::Yes
             ),
    UiAction("action://audio/dev/use-workerrpcmode",
             kfw::ui::UiCtxAny,
             kfw::shortcuts::CTX_DISABLED,
             TranslatableString::untranslatable("Worker RPC mode"),
             Checkable::Yes
             )
};

AudioUiActions::AudioUiActions(const std::shared_ptr<AudioActionsController>& c)
    : m_controller(c)
{
}

const UiActionList& AudioUiActions::actionsList() const
{
    return m_actions;
}

bool AudioUiActions::actionEnabled(const UiAction&) const
{
    return true;
}

async::Channel<ActionCodeList> AudioUiActions::actionEnabledChanged() const
{
    static async::Channel<ActionCodeList> ch;
    return ch;
}

bool AudioUiActions::actionChecked(const UiAction& a) const
{
    return m_controller->actionChecked(a.code);
}

async::Channel<ActionCodeList> AudioUiActions::actionCheckedChanged() const
{
    return m_controller->actionCheckedChanged();
}
