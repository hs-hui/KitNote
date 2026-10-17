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
#ifndef KN_NOTATION_NOTATIONMIDIINPUT_H
#define KN_NOTATION_NOTATIONMIDIINPUT_H

#include <QTimer>

#include "modularity/ioc.h"
#include "playback/iplaybackcontroller.h"
#include "inotationconfiguration.h"
#include "actions/iactionsdispatcher.h"

#include "../inotationmidiinput.h"
#include "igetscore.h"
#include "inotationinteraction.h"
#include "inotationundostack.h"

namespace kn::engraving {
class Score;
}

namespace kn::notation {
class NotationMidiInput : public INotationMidiInput, public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> configuration;
    kfw::ContextInject<playback::IPlaybackController> playbackController = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };

public:
    NotationMidiInput(IGetScore* getScore, INotationInteractionPtr notationInteraction, INotationUndoStackPtr undoStack,
                      const kfw::modularity::ContextPtr& iocCtx);

    void onMidiEventReceived(const kfw::midi::Event& event) override;
    kfw::async::Channel<std::vector<const Note*> > notesReceived() const override;

    void onRealtimeAdvance() override;

private:
    kn::engraving::Score* score() const;

    void doProcessEvents();

    void startNoteInputIfNeed();

    void addNoteEventsToInputState();
    Note* addNoteToScore(const kfw::midi::Event& e);
    Note* makePreviewNote(const kfw::midi::Event& e);

    using ControllerEventMap = std::map<kfw::midi::Event::Opcode, kfw::midi::Event>;
    void triggerControllers(const ControllerEventMap& events);
    void releasePlayingNotes(const std::vector<int>& pitches);

    void enableMetronome();
    void disableMetronome();

    void runRealtime();
    void stopRealtime();

    void doRealtimeAdvance();
    void doExtendCurrentNote();

    NoteInputMethod noteInputMethod() const;
    bool isRealtime() const;
    bool isRealtimeAuto() const;
    bool isRealtimeManual() const;
    bool isInputByDuration() const;

    bool isNoteInputMode() const;

    IGetScore* m_getScore = nullptr;
    INotationInteractionPtr m_notationInteraction;
    INotationUndoStackPtr m_undoStack;
    kfw::async::Channel<std::vector<const Note*> > m_notesReceivedChannel;

    QTimer m_processTimer;
    std::vector<kfw::midi::Event> m_eventsQueue;

    QTimer m_realtimeTimer;
    QTimer m_extendNoteTimer;
    bool m_allowRealtimeRests = false;

    bool m_shouldDisableMetronome = false;
    bool m_holdingNotesInInputByDuration = false;

    struct PlayingNote {
        bool isPreview = false;
        Note* note = nullptr;
    };

    std::map<int, PlayingNote> m_playingNotes;
};
}

#endif // KN_NOTATION_NOTATIONMIDIINPUT_H
