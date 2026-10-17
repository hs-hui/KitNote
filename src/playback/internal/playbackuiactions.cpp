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
#include "playbackuiactions.h"

#include "ui/view/iconcodes.h"
#include "context/uicontext.h"
#include "context/shortcutcontext.h"
#include "types/translatablestring.h"

using namespace kn::playback;
using namespace kn::notation;
using namespace kfw;
using namespace kfw::ui;
using namespace kfw::actions;

static const ActionCode PLAY_FROM_SELECTION_CODE("play-from-selection");

const UiActionList PlaybackUiActions::s_mainActions = {
    UiAction("play",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Play"),
             TranslatableString("action", "Play"),
             IconCode::Code::PLAY
             ),
    UiAction(PLAY_FROM_SELECTION_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Play from selection"),
             TranslatableString("action", "Play from selection"),
             IconCode::Code::PLAY
             ),
    UiAction("pause",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Pause"),
             TranslatableString("action", "Pause playback"),
             IconCode::Code::PAUSE
             ),
    UiAction("pause-and-select",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Pause and select"),
             TranslatableString("action", "Pause and select playback position"),
             IconCode::Code::PAUSE
             ),
    UiAction("stop",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Stop"),
             TranslatableString("action", "Stop playback"),
             IconCode::Code::STOP
             ),
    UiAction("rewind",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Rewind"),
             TranslatableString("action", "Rewind"),
             IconCode::Code::REWIND
             ),
    UiAction("loop",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Loop playback"),
             TranslatableString("action", "Toggle ‘Loop playback’"),
             IconCode::Code::LOOP,
             Checkable::Yes
             ),
    UiAction("metronome",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Metronome"),
             TranslatableString("action", "Toggle metronome playback"),
             IconCode::Code::METRONOME,
             Checkable::Yes
             ),
    UiAction("playback-setup",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Playback setup"),
             TranslatableString("action", "Open playback setup dialog"),
             IconCode::Code::NONE
             ),
    // KitNote: the recording of the song, played in the Mixer along with the score
    UiAction("kitnote-recording",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "Play along with a recordin&g…"),
             TranslatableString("action", "Play a recording of the song along with the score: choose it, and where measure 1 begins in it"),
             IconCode::Code::MICROPHONE
             )
};

const UiActionList PlaybackUiActions::s_midiInputActions = {
    UiAction("midi-on",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Enable MIDI input"),
             TranslatableString("action", "Toggle MIDI input"),
             IconCode::Code::MIDI_INPUT,
             Checkable::Yes
             ),
};

const UiActionList PlaybackUiActions::s_midiInputPitchActions = {
    UiAction("midi-input-written-pitch",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Written pitch"),
             TranslatableString("action", "Input written pitch"),
             IconCode::Code::NONE,
             Checkable::Yes
             ),
    UiAction("midi-input-sounding-pitch",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Sounding pitch"),
             TranslatableString("action", "Input sounding pitch"),
             IconCode::Code::NONE,
             Checkable::Yes
             ),
};

const UiActionList PlaybackUiActions::s_settingsActions = {
    UiAction("repeat",
             kn::context::UiCtxAny,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Play repeats"),
             TranslatableString("action", "Play repeats"),
             IconCode::Code::PLAY_REPEATS,
             Checkable::Yes
             ),
    UiAction("play-chord-symbols",
             kn::context::UiCtxAny,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Play chord symbols"),
             TranslatableString("action", "Play chord symbols"),
             IconCode::Code::CHORD_SYMBOL,
             Checkable::Yes
             ),
    UiAction("toggle-hear-playback-when-editing",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Hear playback when editing"),
             TranslatableString("action", "Toggle hear playback when editing"),
             IconCode::Code::AUDIO,
             Checkable::Yes
             ),
    UiAction("pan",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Pan score automatically"),
             TranslatableString("action", "Pan score automatically during playback"),
             IconCode::Code::PAN_SCORE,
             Checkable::Yes
             ),
    UiAction("countin",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Enable count-in when playing"),
             TranslatableString("action", "Enable count-in when playing"),
             IconCode::Code::COUNT_IN,
             Checkable::Yes
             ),
};

const UiActionList PlaybackUiActions::s_loopBoundaryActions = {
    UiAction("loop-in",
             kn::context::UiCtxAny,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Set loop marker left"),
             TranslatableString("action", "Set loop marker left"),
             IconCode::Code::LOOP_IN
             ),
    UiAction("loop-out",
             kn::context::UiCtxAny,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Set loop marker right"),
             TranslatableString("action", "Set loop marker right"),
             IconCode::Code::LOOP_OUT
             ),
};

const UiActionList PlaybackUiActions::s_diagnosticActions = {
    UiAction("playback-reload-cache",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Reload playback cache")
             )
};

PlaybackUiActions::PlaybackUiActions(std::shared_ptr<PlaybackController> controller, const kfw::modularity::ContextPtr& iocCtx)
    : kfw::Contextable(iocCtx), m_controller(controller)
{
}

void PlaybackUiActions::init()
{
    m_controller->actionCheckedChanged().onReceive(this, [this](const ActionCode& code) {
        m_actionCheckedChanged.send({ code });
    });

    m_controller->isPlayAllowedChanged().onNotify(this, [this]() {
        const UiActionList& actions = actionsList();

        ActionCodeList codes;
        codes.reserve(actions.size());

        for (const UiAction& action : actions) {
            codes.push_back(action.code);
        }

        m_actionEnabledChanged.send(codes);
    });

    globalContext()->currentNotationChanged().onNotify(this, [this]() {
        INotationPtr currNotation = globalContext()->currentNotation();
        if (!currNotation) {
            return;
        }

        INotationInteractionPtr interaction = currNotation->interaction();
        interaction->selectionChanged().onNotify(this, [this]() {
            m_actionEnabledChanged.send({ PLAY_FROM_SELECTION_CODE });
        }, Asyncable::Mode::SetReplace /* FIXME */);

        interaction->isEditingElementChanged().onNotify(this, [this]() {
            m_actionEnabledChanged.send({ PLAY_FROM_SELECTION_CODE });
        }, Asyncable::Mode::SetReplace /* FIXME */);
    });
}

const UiActionList& PlaybackUiActions::actionsList() const
{
    static UiActionList alist;
    if (alist.empty()) {
        alist.insert(alist.end(), s_mainActions.cbegin(), s_mainActions.cend());
        alist.insert(alist.end(), s_midiInputActions.cbegin(), s_midiInputActions.cend());
        alist.insert(alist.end(), s_midiInputPitchActions.cbegin(), s_midiInputPitchActions.cend());
        alist.insert(alist.end(), s_settingsActions.cbegin(), s_settingsActions.cend());
        alist.insert(alist.end(), s_loopBoundaryActions.cbegin(), s_loopBoundaryActions.cend());
        alist.insert(alist.end(), s_diagnosticActions.cbegin(), s_diagnosticActions.cend());
    }
    return alist;
}

bool PlaybackUiActions::actionEnabled(const UiAction& act) const
{
    if (!m_controller->isPlayAllowed()) {
        return false;
    }

    if (!m_controller->canReceiveAction(act.code)) {
        return false;
    }

    if (act.code == PLAY_FROM_SELECTION_CODE) {
        const INotationPtr currNotation = globalContext()->currentNotation();
        const INotationInteractionPtr interaction = currNotation ? currNotation->interaction() : nullptr;
        return interaction && !interaction->isEditingElement();
    }

    return true;
}

bool PlaybackUiActions::actionChecked(const UiAction& act) const
{
    return m_controller->actionChecked(act.code);
}

kfw::async::Channel<ActionCodeList> PlaybackUiActions::actionEnabledChanged() const
{
    return m_actionEnabledChanged;
}

kfw::async::Channel<ActionCodeList> PlaybackUiActions::actionCheckedChanged() const
{
    return m_actionCheckedChanged;
}

const UiActionList& PlaybackUiActions::midiInputActions()
{
    return s_midiInputActions;
}

const UiActionList& PlaybackUiActions::midiInputPitchActions()
{
    return s_midiInputPitchActions;
}

const UiActionList& PlaybackUiActions::settingsActions()
{
    return s_settingsActions;
}

const UiActionList& PlaybackUiActions::loopBoundaryActions()
{
    return s_loopBoundaryActions;
}

const kfw::ui::ToolConfig& PlaybackUiActions::defaultPlaybackToolConfig()
{
    static ToolConfig config;
    if (!config.isValid()) {
        config.items = {
            { "rewind", true },
            { "play", true },
            { "loop", true },
            { "loop-in", true },
            { "loop-out", true },
            { "metronome", true },
        };
    }
    return config;
}
