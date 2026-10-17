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

#include <QString>

#include "modularity/imoduleinterface.h"
#include "async/notification.h"
#include "async/channel.h"
#include "async/promise.h"
#include "global/progress.h"
#include "notation/inotation.h"
#include "notation/notationtypes.h"
#include "audio/common/audiotypes.h"
#include "actions/actiontypes.h"
#include "midi/miditypes.h"

#include "playbacktypes.h"

namespace kn::playback {
class IPlaybackController : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IPlaybackController)

public:
    virtual ~IPlaybackController() = default;

    virtual bool isPlayAllowed() const = 0;
    virtual kfw::async::Notification isPlayAllowedChanged() const = 0;

    virtual bool isPlaying() const = 0;
    virtual kfw::async::Notification isPlayingChanged() const = 0;

    virtual void reset() = 0;

    virtual kfw::async::Channel<kfw::audio::secs_t, kfw::midi::tick_t> currentPlaybackPositionChanged() const = 0;

    virtual kfw::audio::TrackSequenceId currentTrackSequenceId() const = 0;
    virtual kfw::async::Notification currentTrackSequenceIdChanged() const = 0;

    using InstrumentTrackIdMap = std::unordered_map<engraving::InstrumentTrackId, kfw::audio::TrackId>;
    virtual const InstrumentTrackIdMap& instrumentTrackIdMap() const = 0;

    using AuxTrackIdMap = std::map<kfw::audio::aux_channel_idx_t, kfw::audio::TrackId>;
    virtual const AuxTrackIdMap& auxTrackIdMap() const = 0;

    virtual kfw::async::Channel<kfw::audio::TrackId> trackAdded() const = 0;
    virtual kfw::async::Channel<kfw::audio::TrackId> trackRemoved() const = 0;

    virtual std::string auxChannelName(kfw::audio::aux_channel_idx_t index) const = 0;
    virtual kfw::async::Channel<kfw::audio::aux_channel_idx_t, std::string> auxChannelNameChanged() const = 0;

    virtual kfw::async::Promise<kfw::audio::SoundPresetList> availableSoundPresets(const engraving::InstrumentTrackId& instrumentTrackId)
    const
        = 0;

    using SoloMuteState = notation::INotationSoloMuteState::SoloMuteState;

    virtual const SoloMuteState& trackSoloMuteState(const engraving::InstrumentTrackId& trackId) const = 0;
    virtual void setTrackSoloMuteState(const engraving::InstrumentTrackId& trackId, const SoloMuteState& state) = 0;

    struct PlayParams {
        PlayParams() {}

        std::optional<kfw::mpe::duration_t> duration; // no duration -> use default
        bool flushSound = true;
    };

    virtual void playElements(const std::vector<const notation::EngravingItem*>& elements,
                              const PlayParams& params = PlayParams(), bool isMidi = false) = 0;
    virtual void playNotes(const notation::NoteValList& notes, notation::staff_idx_t staffIdx, const notation::Segment* segment,
                           const PlayParams& params = PlayParams()) = 0;
    virtual void playMetronome(int tick) = 0;

    virtual void triggerControllers(const kfw::mpe::ControllerChangeEventList& list, notation::staff_idx_t staffIdx, int tick) = 0;

    virtual void seekElement(const notation::EngravingItem* element, bool flushSound = true) = 0;
    virtual void seekBeat(int measureIndex, int beatIndex, bool flushSound = true) = 0;

    virtual bool actionChecked(const kfw::actions::ActionCode& actionCode) const = 0;
    virtual kfw::async::Channel<kfw::actions::ActionCode> actionCheckedChanged() const = 0;

    virtual kfw::secs_t totalPlayTime() const = 0;
    virtual kfw::async::Notification totalPlayTimeChanged() const = 0;

    virtual const notation::Tempo& currentTempo() const = 0;
    virtual kfw::async::Notification currentTempoChanged() const = 0;

    virtual notation::MeasureBeat currentBeat() const = 0;
    virtual kfw::audio::secs_t beatToSecs(int measureIndex, int beatIndex) const = 0;

    virtual double tempoMultiplier() const = 0;
    virtual void setTempoMultiplier(double multiplier) = 0;

    virtual kfw::Progress loadingProgress() const = 0;

    virtual void applyProfile(const SoundProfileName& profileName) = 0;

    virtual void setNotation(notation::INotationPtr notation) = 0;
    virtual void setIsExportingAudio(bool exporting) = 0;

    virtual const std::map<kfw::audio::TrackId, kfw::audio::AudioResourceMeta>& onlineSounds() const = 0;
    virtual kfw::async::Notification onlineSoundsChanged() const = 0;
    virtual kfw::Progress onlineSoundsProcessingProgress() const = 0;

    // KitNote: the recording of the score (kitnoterecording.h), played in the Mixer's "Recording" channel
    struct RecordingState {
        bool attached = false;          // the score has a recording
        QString filePath;               // the file found for it (empty: not found)
        bool loaded = false;            // read: it plays along
        bool failed = false;            // not found, or not a file KitNote can play
        bool mutedBySpeed = false;      // the playback speed is not 100%: it would not fit the score
        double durationSecs = 0.0;
        double offsetSecs = 0.0;        // where measure 1 begins in it
    };
    virtual const RecordingState& recordingState() const = 0;
    virtual kfw::async::Notification recordingStateChanged() const = 0;
    //! looks again for a recording that was not found (or could not be read): it may be there now
    virtual void recheckRecording() = 0;
};
}
