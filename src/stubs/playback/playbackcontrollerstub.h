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

#pragma once

#include "playback/iplaybackcontroller.h"

namespace kn::playback {
class PlaybackControllerStub : public IPlaybackController
{
public:
    bool isPlayAllowed() const override;
    kfw::async::Notification isPlayAllowedChanged() const override;

    bool isPlaying() const override;
    kfw::async::Notification isPlayingChanged() const override;

    void reset() override;

    kfw::async::Channel<kfw::audio::secs_t, kfw::midi::tick_t> currentPlaybackPositionChanged() const override;

    kfw::audio::TrackSequenceId currentTrackSequenceId() const override;
    kfw::async::Notification currentTrackSequenceIdChanged() const override;

    const InstrumentTrackIdMap& instrumentTrackIdMap() const override;
    const AuxTrackIdMap& auxTrackIdMap() const override;

    kfw::async::Channel<kfw::audio::TrackId> trackAdded() const override;
    kfw::async::Channel<kfw::audio::TrackId> trackRemoved() const override;

    std::string auxChannelName(kfw::audio::aux_channel_idx_t index) const override;
    kfw::async::Channel<kfw::audio::aux_channel_idx_t, std::string> auxChannelNameChanged() const override;

    kfw::async::Promise<kfw::audio::SoundPresetList> availableSoundPresets(
        const engraving::InstrumentTrackId& instrumentTrackId) const override;

    const SoloMuteState& trackSoloMuteState(const engraving::InstrumentTrackId& trackId) const override;
    void setTrackSoloMuteState(const engraving::InstrumentTrackId& trackId, const SoloMuteState& state) override;

    void playElements(const std::vector<const notation::EngravingItem*>& elements,
                      const PlayParams& params = PlayParams(), bool isMidi = false) override;
    void playNotes(const notation::NoteValList& notes, notation::staff_idx_t staffIdx, const notation::Segment* segment,
                   const PlayParams& params = PlayParams()) override;
    void playMetronome(int tick) override;

    void triggerControllers(const kfw::mpe::ControllerChangeEventList& list, notation::staff_idx_t staffIdx, int tick) override;

    void seekElement(const notation::EngravingItem* element, bool flushSound = true) override;
    void seekBeat(int measureIndex, int beatIndex, bool flushSound = true) override;

    bool actionChecked(const kfw::actions::ActionCode& actionCode) const override;
    kfw::async::Channel<kfw::actions::ActionCode> actionCheckedChanged() const override;

    kfw::secs_t totalPlayTime() const override;
    kfw::async::Notification totalPlayTimeChanged() const override;

    const notation::Tempo& currentTempo() const override;
    kfw::async::Notification currentTempoChanged() const override;

    notation::MeasureBeat currentBeat() const override;
    kfw::audio::secs_t beatToSecs(int measureIndex, int beatIndex) const override;

    double tempoMultiplier() const override;
    void setTempoMultiplier(double multiplier) override;

    kfw::Progress loadingProgress() const override;

    void applyProfile(const SoundProfileName& profileName) override;

    void setNotation(notation::INotationPtr notation) override;
    void setIsExportingAudio(bool exporting) override;

    const std::map<kfw::audio::TrackId, kfw::audio::AudioResourceMeta>& onlineSounds() const override;
    kfw::async::Notification onlineSoundsChanged() const override;
    kfw::Progress onlineSoundsProcessingProgress() const override;

    const RecordingState& recordingState() const override;
    kfw::async::Notification recordingStateChanged() const override;
    void recheckRecording() override;
};
}
