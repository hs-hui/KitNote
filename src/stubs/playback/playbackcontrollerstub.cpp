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
#include "playbackcontrollerstub.h"

using namespace kn::playback;
using namespace kfw::actions;

bool PlaybackControllerStub::isPlayAllowed() const
{
    return false;
}

kfw::async::Notification PlaybackControllerStub::isPlayAllowedChanged() const
{
    return kfw::async::Notification();
}

bool PlaybackControllerStub::isPlaying() const
{
    return false;
}

kfw::async::Notification PlaybackControllerStub::isPlayingChanged() const
{
    return kfw::async::Notification();
}

void PlaybackControllerStub::reset()
{
}

kfw::async::Channel<kfw::audio::secs_t, kfw::midi::tick_t> PlaybackControllerStub::currentPlaybackPositionChanged() const
{
    return kfw::async::Channel<kfw::audio::secs_t, kfw::midi::tick_t>();
}

kfw::audio::TrackSequenceId PlaybackControllerStub::currentTrackSequenceId() const
{
    return 0;
}

kfw::async::Notification PlaybackControllerStub::currentTrackSequenceIdChanged() const
{
    return kfw::async::Notification();
}

const IPlaybackController::InstrumentTrackIdMap& PlaybackControllerStub::instrumentTrackIdMap() const
{
    static const InstrumentTrackIdMap m;
    return m;
}

const IPlaybackController::AuxTrackIdMap& PlaybackControllerStub::auxTrackIdMap() const
{
    static const AuxTrackIdMap m;
    return m;
}

kfw::async::Channel<kfw::audio::TrackId> PlaybackControllerStub::trackAdded() const
{
    return {};
}

kfw::async::Channel<kfw::audio::TrackId> PlaybackControllerStub::trackRemoved() const
{
    return {};
}

std::string PlaybackControllerStub::auxChannelName(kfw::audio::aux_channel_idx_t) const
{
    return "";
}

kfw::async::Channel<kfw::audio::aux_channel_idx_t, std::string> PlaybackControllerStub::auxChannelNameChanged() const
{
    return {};
}

kfw::async::Promise<kfw::audio::SoundPresetList> PlaybackControllerStub::availableSoundPresets(const engraving::InstrumentTrackId&) const
{
    return kfw::async::Promise<kfw::audio::SoundPresetList>([](auto /*resolve*/, auto reject) {
        return reject(int(kfw::Ret::Code::UnknownError), "stub");
    });
}

const PlaybackControllerStub::SoloMuteState& PlaybackControllerStub::trackSoloMuteState(const engraving::InstrumentTrackId&) const
{
    static const SoloMuteState state;
    return state;
}

void PlaybackControllerStub::setTrackSoloMuteState(const engraving::InstrumentTrackId&, const SoloMuteState&)
{
}

void PlaybackControllerStub::playElements(const std::vector<const notation::EngravingItem*>&, const PlayParams&, bool)
{
}

void PlaybackControllerStub::playNotes(const notation::NoteValList&, notation::staff_idx_t, const notation::Segment*,
                                       const PlayParams&)
{
}

void PlaybackControllerStub::playMetronome(int)
{
}

void PlaybackControllerStub::triggerControllers(const kfw::mpe::ControllerChangeEventList&, notation::staff_idx_t, int)
{
}

void PlaybackControllerStub::seekElement(const notation::EngravingItem*, bool)
{
}

void PlaybackControllerStub::seekBeat(int, int, bool)
{
}

bool PlaybackControllerStub::actionChecked(const ActionCode&) const
{
    return false;
}

kfw::async::Channel<ActionCode> PlaybackControllerStub::actionCheckedChanged() const
{
    return {};
}

kfw::secs_t PlaybackControllerStub::totalPlayTime() const
{
    return kfw::secs_t { 0.0 };
}

kfw::async::Notification PlaybackControllerStub::totalPlayTimeChanged() const
{
    return {};
}

const kn::notation::Tempo& PlaybackControllerStub::currentTempo() const
{
    static const kn::notation::Tempo dummyTempo;
    return dummyTempo;
}

kfw::async::Notification PlaybackControllerStub::currentTempoChanged() const
{
    return {};
}

kn::notation::MeasureBeat PlaybackControllerStub::currentBeat() const
{
    return {};
}

kfw::audio::secs_t PlaybackControllerStub::beatToSecs(int, int) const
{
    return 0;
}

double PlaybackControllerStub::tempoMultiplier() const
{
    return 1.0;
}

void PlaybackControllerStub::setTempoMultiplier(double)
{
}

kfw::Progress PlaybackControllerStub::loadingProgress() const
{
    return {};
}

void PlaybackControllerStub::applyProfile(const SoundProfileName&)
{
}

void PlaybackControllerStub::setNotation(notation::INotationPtr)
{
}

void PlaybackControllerStub::setIsExportingAudio(bool)
{
}

const std::map<kfw::audio::TrackId, kfw::audio::AudioResourceMeta>& PlaybackControllerStub::onlineSounds() const
{
    static const std::map<kfw::audio::TrackId, kfw::audio::AudioResourceMeta> dummy;
    return dummy;
}

kfw::async::Notification PlaybackControllerStub::onlineSoundsChanged() const
{
    return {};
}

kfw::Progress PlaybackControllerStub::onlineSoundsProcessingProgress() const
{
    return {};
}

const IPlaybackController::RecordingState& PlaybackControllerStub::recordingState() const
{
    static const RecordingState dummy;
    return dummy;
}

kfw::async::Notification PlaybackControllerStub::recordingStateChanged() const
{
    return {};
}

void PlaybackControllerStub::recheckRecording()
{
}
