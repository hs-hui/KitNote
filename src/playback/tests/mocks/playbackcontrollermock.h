/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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
#ifndef KN_PLAYBACK_PLAYBACKCONTROLLERMOCK_H
#define KN_PLAYBACK_PLAYBACKCONTROLLERMOCK_H

#include <gmock/gmock.h>

#include "playback/iplaybackcontroller.h"

namespace kn::playback {
class PlaybackControllerMock : public IPlaybackController
{
public:
    MOCK_METHOD(bool, isPlayAllowed, (), (const, override));
    MOCK_METHOD(kfw::async::Notification, isPlayAllowedChanged, (), (const, override));

    MOCK_METHOD(bool, isPlaying, (), (const, override));
    MOCK_METHOD(kfw::async::Notification, isPlayingChanged, (), (const, override));

    MOCK_METHOD(void, reset, (), (override));

    MOCK_METHOD((kfw::async::Channel<kfw::audio::secs_t, kfw::midi::tick_t>), currentPlaybackPositionChanged, (), (const, override));

    MOCK_METHOD(kfw::audio::TrackSequenceId, currentTrackSequenceId, (), (const, override));
    MOCK_METHOD(kfw::async::Notification, currentTrackSequenceIdChanged, (), (const, override));

    MOCK_METHOD(const InstrumentTrackIdMap&, instrumentTrackIdMap, (), (const, override));

    MOCK_METHOD(const AuxTrackIdMap&, auxTrackIdMap, (), (const, override));

    MOCK_METHOD(kfw::async::Channel<kfw::audio::TrackId>, trackAdded, (), (const, override));
    MOCK_METHOD(kfw::async::Channel<kfw::audio::TrackId>, trackRemoved, (), (const, override));

    MOCK_METHOD(std::string, auxChannelName, (kfw::audio::aux_channel_idx_t), (const, override));
    MOCK_METHOD((kfw::async::Channel<kfw::audio::aux_channel_idx_t, std::string>), auxChannelNameChanged, (), (const, override));

    MOCK_METHOD(kfw::async::Promise<kfw::audio::SoundPresetList>, availableSoundPresets, (const engraving::InstrumentTrackId&), (const,
                                                                                                                                   override));

    MOCK_METHOD(const SoloMuteState&, trackSoloMuteState, (const engraving::InstrumentTrackId&),
                (const, override));
    MOCK_METHOD(void, setTrackSoloMuteState, (const engraving::InstrumentTrackId&, const SoloMuteState&),
                (override));

    MOCK_METHOD(void, playElements, (const std::vector<const notation::EngravingItem*>&, const PlayParams&, bool), (override));
    MOCK_METHOD(void, playNotes, (const notation::NoteValList&, notation::staff_idx_t, const notation::Segment*, const PlayParams&),
                (override));
    MOCK_METHOD(void, playMetronome, (int), (override));

    MOCK_METHOD(void, triggerControllers, (const kfw::mpe::ControllerChangeEventList&, notation::staff_idx_t, int), (override));

    MOCK_METHOD(void, seekElement, (const notation::EngravingItem*, bool), (override));
    MOCK_METHOD(void, seekBeat, (int, int, bool), (override));

    MOCK_METHOD(bool, actionChecked, (const kfw::actions::ActionCode&), (const, override));
    MOCK_METHOD(kfw::async::Channel<kfw::actions::ActionCode>, actionCheckedChanged, (), (const, override));

    MOCK_METHOD(kfw::secs_t, totalPlayTime, (), (const, override));
    MOCK_METHOD(kfw::async::Notification, totalPlayTimeChanged, (), (const, override));

    MOCK_METHOD(const notation::Tempo&, currentTempo, (), (const, override));
    MOCK_METHOD(kfw::async::Notification, currentTempoChanged, (), (const, override));

    MOCK_METHOD(notation::MeasureBeat, currentBeat, (), (const, override));
    MOCK_METHOD(kfw::audio::secs_t, beatToSecs, (int, int), (const, override));

    MOCK_METHOD(double, tempoMultiplier, (), (const, override));
    MOCK_METHOD(void, setTempoMultiplier, (double), (override));

    MOCK_METHOD(kfw::Progress, loadingProgress, (), (const, override));

    MOCK_METHOD(void, applyProfile, (const SoundProfileName&), (override));

    MOCK_METHOD(void, setNotation, (notation::INotationPtr), (override));
    MOCK_METHOD(void, setIsExportingAudio, (bool), (override));

    MOCK_METHOD((const std::map<kfw::audio::TrackId, kfw::audio::AudioResourceMeta>&), onlineSounds, (), (const, override));
    MOCK_METHOD(kfw::async::Notification, onlineSoundsChanged, (), (const, override));
    MOCK_METHOD(kfw::Progress, onlineSoundsProcessingProgress, (), (const, override));

    MOCK_METHOD(const RecordingState&, recordingState, (), (const, override));
    MOCK_METHOD(kfw::async::Notification, recordingStateChanged, (), (const, override));
    MOCK_METHOD(void, recheckRecording, (), (override));
};
}

#endif // KN_PLAYBACK_PLAYBACKCONTROLLERMOCK_H
