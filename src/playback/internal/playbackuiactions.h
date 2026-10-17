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
#ifndef KN_PLAYBACK_PLAYBACKUIACTIONS_H
#define KN_PLAYBACK_PLAYBACKUIACTIONS_H

#include "ui/iuiactionsmodule.h"
#include "playbackcontroller.h"
#include "modularity/ioc.h"
#include "context/iuicontextresolver.h"
#include "context/iglobalcontext.h"
#include "async/asyncable.h"
#include "ui/uitypes.h"

namespace kn::playback {
class PlaybackUiActions : public kfw::ui::IUiActionsModule, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::ContextInject<context::IUiContextResolver> uicontextResolver = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    PlaybackUiActions(std::shared_ptr<PlaybackController> controller, const kfw::modularity::ContextPtr& iocCtx);

    void init();

    const kfw::ui::UiActionList& actionsList() const override;

    bool actionEnabled(const kfw::ui::UiAction& act) const override;
    kfw::async::Channel<kfw::actions::ActionCodeList> actionEnabledChanged() const override;

    bool actionChecked(const kfw::ui::UiAction& act) const override;
    kfw::async::Channel<kfw::actions::ActionCodeList> actionCheckedChanged() const override;

    static const kfw::ui::UiActionList& midiInputActions();
    static const kfw::ui::UiActionList& midiInputPitchActions();
    static const kfw::ui::UiActionList& settingsActions();
    static const kfw::ui::UiActionList& loopBoundaryActions();

    static const kfw::ui::ToolConfig& defaultPlaybackToolConfig();

private:
    static const kfw::ui::UiActionList s_mainActions;
    static const kfw::ui::UiActionList s_midiInputActions;
    static const kfw::ui::UiActionList s_midiInputPitchActions;
    static const kfw::ui::UiActionList s_settingsActions;
    static const kfw::ui::UiActionList s_loopBoundaryActions;
    static const kfw::ui::UiActionList s_diagnosticActions;

    std::shared_ptr<PlaybackController> m_controller;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionEnabledChanged;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionCheckedChanged;
};
}

#endif // KN_PLAYBACK_PLAYBACKUIACTIONS_H
