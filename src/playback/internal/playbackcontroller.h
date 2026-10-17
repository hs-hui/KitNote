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

#include "modularity/ioc.h"
#include "async/asyncable.h"
#include "actions/iactionsdispatcher.h"
#include "actions/actionable.h"
#include "context/iglobalcontext.h"
#include "notation/notationtypes.h"
#include "engraving/types/types.h"
#include "notation/inotationconfiguration.h"
#include "notation/inotationplayback.h"
#include "audio/main/iplayer.h"
#include "audio/main/iplayback.h"
#include "audio/common/audiotypes.h"
#include "iinteractive.h"
#include "iapplication.h"
#include "tours/itoursservice.h"

#include "drumsetloader.h"

#include "../iplaybackcontroller.h"
#include "../iplaybackconfiguration.h"
#include "../isoundprofilesrepository.h"

namespace kn::playback {
class OnlineSoundsController;
class PlaybackController : public IPlaybackController, public kfw::actions::Actionable, public kfw::async::Asyncable,
    public kfw::Contextable
{
    kfw::GlobalInject<IPlaybackConfiguration> configuration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::audio::IPlayback> playback = { this };
    kfw::ContextInject<ISoundProfilesRepository> profilesRepo = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<kfw::tours::IToursService> tours = { this };
    kfw::ContextInject<kfw::IApplication> application = { this };

public:
    PlaybackController(const kfw::modularity::ContextPtr& iocCtx);
    ~PlaybackController() override;

    void init();

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
    void setMasterNotation(notation::IMasterNotationPtr masterNotation);

    void setIsExportingAudio(bool exporting) override;

    bool canReceiveAction(const kfw::actions::ActionCode& code) const override;

    const std::map<kfw::audio::TrackId, kfw::audio::AudioResourceMeta>& onlineSounds() const override;
    kfw::async::Notification onlineSoundsChanged() const override;
    kfw::Progress onlineSoundsProcessingProgress() const override;

    const RecordingState& recordingState() const override;
    kfw::async::Notification recordingStateChanged() const override;
    void recheckRecording() override;

private:
    kfw::audio::IPlayerPtr currentPlayer() const;

    notation::INotationPlaybackPtr notationPlayback() const;
    notation::INotationPartsPtr masterNotationParts() const;
    notation::INotationSelectionPtr selection() const;
    notation::INotationSelectionRangePtr selectionRange() const;
    notation::INotationInteractionPtr interaction() const;

    uint64_t notationPlaybackKey() const;

    void updateCurrentTempo();

    void seekRawTick(const kfw::midi::tick_t tick, const bool flushSound = true);
    void seek(const kfw::audio::secs_t secs, const bool flushSound = true);

    bool isPaused() const;
    bool isLoaded() const;

    bool isLoopEnabled() const;
    bool loopBoundariesSet() const;

    void onNotationChanged();
    void onPartChanged(const engraving::Part* part);
    void onPlaybackStatusChanged();

    void onSelectionChanged();
    void seekRangeSelection();

    void onAudioResourceChanged(const kfw::audio::TrackId trackId, const kn::engraving::InstrumentTrackId& instrumentTrackId,
                                const kfw::audio::AudioResourceMeta& oldMeta, const kfw::audio::AudioResourceMeta& newMeta);

    bool shouldLoadDrumset(const engraving::InstrumentTrackId& instrumentTrackId, const kfw::audio::AudioResourceMeta& oldMeta,
                           const kfw::audio::AudioResourceMeta& newMeta) const;

    void addSoundFlagsIfNeed(const std::vector<engraving::EngravingItem*>& selection);

    void togglePlay(bool showErrors = true);
    void playFromSelection(bool showErrors = true);
    void rewind(const kfw::actions::ActionData& args);
    void play();
    void pause(bool select = false);
    void stop();
    void resume();

    kfw::audio::secs_t playbackStartSecs() const;

    notation::InstrumentTrackIdSet instrumentTrackIdSetForRangePlayback() const;

    void togglePlayRepeats();
    void togglePlayChordSymbols();
    void toggleAutomaticallyPan();
    void toggleMetronome();
    void toggleCountIn();
    void toggleMidiInput();
    void setMidiUseWrittenPitch(bool useWrittenPitch);
    void toggleLoopPlayback();
    void toggleHearPlaybackWhenEditing();

    void reloadPlaybackCache();

    void openPlaybackSetupDialog();

    void addLoopBoundary(notation::LoopBoundaryType type);
    void addLoopBoundaryToTick(notation::LoopBoundaryType type, int tick);
    void updateLoop();

    void enableLoop();
    void disableLoop();

    void notifyActionCheckedChanged(const kfw::actions::ActionCode& actionCode);

    project::IProjectAudioSettingsPtr audioSettings() const;

    void resetCurrentSequence();
    void setupNewCurrentSequence(const kfw::audio::TrackSequenceId sequenceId);
    void subscribeOnAudioParamsChanges();
    void setupSequenceTracks();
    void setupSequencePlayer();

    void updateSoloMuteStates();
    void updateAuxMuteStates();

    using TrackAddFinished = std::function<void ()>;

    void addTrack(const engraving::InstrumentTrackId& instrumentTrackId, const TrackAddFinished& onFinished);
    void doAddTrack(const engraving::InstrumentTrackId& instrumentTrackId, const std::string& title, const TrackAddFinished& onFinished);
    void addAuxTrack(kfw::audio::aux_channel_idx_t index, const TrackAddFinished& onFinished);

    void setTrackActivity(const engraving::InstrumentTrackId& instrumentTrackId, const bool isActive);
    kfw::audio::AudioOutputParams trackOutputParams(const engraving::InstrumentTrackId& instrumentTrackId) const;
    void removeTrack(const engraving::InstrumentTrackId& instrumentTrackId);

    void onTrackNewlyAdded(const engraving::InstrumentTrackId& instrumentTrackId);

    kfw::audio::secs_t playedTickToSecs(int tick) const;

    // KitNote: the recording of the score, in the Mixer's "Recording" channel
    void updateRecordingTrack();
    void addRecordingTrack(const QString& filePath);
    void removeRecordingTrack();
    kfw::audio::AudioInputParams recordingInputParams() const;
    void onRecordingInputParamsChanged(const kfw::audio::AudioInputParams& params);
    void setRecordingState(const RecordingState& state);

    notation::INotationPtr m_notation;
    notation::IMasterNotationPtr m_masterNotation;
    kfw::audio::IPlayerPtr m_player;

    kfw::async::Notification m_isPlayAllowedChanged;
    kfw::async::Notification m_isPlayingChanged;
    kfw::async::Notification m_totalPlayTimeChanged;
    kfw::async::Notification m_currentTempoChanged;
    kfw::async::Channel<kfw::audio::secs_t, kfw::midi::tick_t> m_currentPlaybackPositionChanged;
    kfw::async::Channel<kfw::actions::ActionCode> m_actionCheckedChanged;

    kfw::audio::TrackSequenceId m_currentSequenceId = -1;

    kfw::async::Notification m_currentSequenceIdChanged;
    kfw::midi::tick_t m_currentTick = 0;
    notation::Tempo m_currentTempo;

    kfw::async::Channel<kfw::audio::TrackId> m_trackAdded;
    kfw::async::Channel<kfw::audio::TrackId> m_trackRemoved;

    kfw::async::Channel<kfw::audio::aux_channel_idx_t, std::string> m_auxChannelNameChanged;

    kfw::async::Asyncable m_seqAsyncReceiver; //! HACK - see PlaybackController::setupSequenceTracks

    InstrumentTrackIdMap m_instrumentTrackIdMap;
    AuxTrackIdMap m_auxTrackIdMap;

    kfw::Progress m_loadingProgress;
    size_t m_loadingTrackCount = 0;

    bool m_isExportingAudio = false;
    bool m_isRangeSelection = false;

    DrumsetLoader m_drumsetLoader;
    std::unique_ptr<OnlineSoundsController> m_onlineSoundsController;

    bool m_measureInputLag = false;

    RecordingState m_recordingState;
    kfw::async::Notification m_recordingStateChanged;
    QString m_recordingFile;                // the file being played (resolved)
    QString m_recordingStoredPath;          // the path the score gives, the score's own, and the file they lead to
    QString m_recordingScorePath;
    QString m_recordingFound;
    kfw::audio::msecs_t m_recordingEngineOffset = 0;
    uint64_t m_recordingRequest = 0;        // the last recording asked for: an older answer is dropped
    QString m_recordingRechecked;           // the file (and its size and time) last tried again by recheckRecording
};
}
