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
#include "playbackcontroller.h"

#include "async/notifylist.h"

#include "modularity/ioc.h"
#include "onlinesoundscontroller.h"

#include "playbacktypes.h"
#include "exportmetronomesetting.h"
#include "kitnoterecording.h" // KitNote

#include "engraving/dom/masterscore.h"
#include "engraving/dom/stafftext.h"
#include "engraving/dom/utils.h"
#include "engraving/dom/factory.h"

#include "audio/common/audioutils.h"
#include "global/io/buffer.h" // KitNote
#include "audio/devtools/inputlag.h"

#include "containers.h"
#include "defer.h"
#include "log.h"

#include <QDateTime> // KitNote
#include <QFile> // KitNote
#include <QFileInfo> // KitNote

using namespace kfw;
using namespace kfw::actions;
using namespace kfw::async;
using namespace kfw::audio;
using namespace kfw::midi;
using namespace kn::engraving;
using namespace kn::notation;
using namespace kn::playback;

static const ActionCode PLAY_CODE("play");
static const ActionCode PLAY_FROM_SELECTION("play-from-selection");
static const ActionCode PAUSE_CODE("pause");
static const ActionCode STOP_CODE("stop");
static const ActionCode PAUSE_AND_SELECT_CODE("pause-and-select");
static const ActionCode REWIND_CODE("rewind");
static const ActionCode LOOP_CODE("loop");
static const ActionCode LOOP_IN_CODE("loop-in");
static const ActionCode LOOP_OUT_CODE("loop-out");
static const ActionCode METRONOME_CODE("metronome");
static const ActionCode MIDI_ON_CODE("midi-on");
static const ActionCode INPUT_WRITTEN_PITCH("midi-input-written-pitch");
static const ActionCode INPUT_SOUNDING_PITCH("midi-input-sounding-pitch");
static const ActionCode COUNT_IN_CODE("countin");
static const ActionCode PAN_CODE("pan");
static const ActionCode REPEAT_CODE("repeat");
static const ActionCode PLAY_CHORD_SYMBOLS_CODE("play-chord-symbols");
static const ActionCode PLAYBACK_SETUP("playback-setup");
static const ActionCode TOGGLE_HEAR_PLAYBACK_WHEN_EDITING_CODE("toggle-hear-playback-when-editing");

static AudioOutputParams makeReverbOutputParams()
{
    AudioFxParams reverbParams;
    reverbParams.resourceMeta = makeReverbMeta();
    reverbParams.categories.insert(AudioFxCategory::FxReverb);
    reverbParams.chainOrder = 0;
    reverbParams.active = true;

    AudioOutputParams result;
    result.fxChain.emplace(reverbParams.chainOrder, std::move(reverbParams));

    return result;
}

static std::string resolveAuxTrackTitle(aux_channel_idx_t index, const AudioOutputParams& params, bool considerFx = true)
{
    if (considerFx && params.fxChain.size() == 1) {
        const AudioResourceMeta& meta = params.fxChain.cbegin()->second.resourceMeta;
        if (meta.id == KFW_REVERB_ID) {
            return kfw::trc("playback", "Reverb");
        }

        return meta.id;
    }

    return kfw::mtrc("playback", "Aux %1").arg(index + 1).toStdString();
}

PlaybackController::PlaybackController(const kfw::modularity::ContextPtr& iocCtx)
    : kfw::Contextable(iocCtx), m_drumsetLoader(iocCtx), m_onlineSoundsController(std::make_unique<OnlineSoundsController>(iocCtx))
{
}

PlaybackController::~PlaybackController() = default;

void PlaybackController::init()
{
    dispatcher()->reg(this, PLAY_CODE, [this]() { PlaybackController::togglePlay(); });
    dispatcher()->reg(this, PLAY_FROM_SELECTION, [this]() { PlaybackController::playFromSelection(); });
    dispatcher()->reg(this, PAUSE_CODE, [this]() { PlaybackController::pause(/*select*/ false); });
    dispatcher()->reg(this, PAUSE_AND_SELECT_CODE, [this]() { PlaybackController::pause(/*select*/ true); });
    dispatcher()->reg(this, STOP_CODE, this, &PlaybackController::stop);
    dispatcher()->reg(this, REWIND_CODE, this, &PlaybackController::rewind);
    dispatcher()->reg(this, LOOP_CODE, this, &PlaybackController::toggleLoopPlayback);
    dispatcher()->reg(this, LOOP_IN_CODE, [this]() { addLoopBoundary(LoopBoundaryType::LoopIn); });
    dispatcher()->reg(this, LOOP_OUT_CODE, [this]() { addLoopBoundary(LoopBoundaryType::LoopOut); });
    dispatcher()->reg(this, REPEAT_CODE, this, &PlaybackController::togglePlayRepeats);
    dispatcher()->reg(this, PLAY_CHORD_SYMBOLS_CODE, this, &PlaybackController::togglePlayChordSymbols);
    dispatcher()->reg(this, PAN_CODE, this, &PlaybackController::toggleAutomaticallyPan);
    dispatcher()->reg(this, METRONOME_CODE, this, &PlaybackController::toggleMetronome);
    dispatcher()->reg(this, COUNT_IN_CODE, this, &PlaybackController::toggleCountIn);
    dispatcher()->reg(this, MIDI_ON_CODE, this, &PlaybackController::toggleMidiInput);
    dispatcher()->reg(this, INPUT_WRITTEN_PITCH, [this]() { PlaybackController::setMidiUseWrittenPitch(true); });
    dispatcher()->reg(this, INPUT_SOUNDING_PITCH, [this]() { PlaybackController::setMidiUseWrittenPitch(false); });
    dispatcher()->reg(this, PLAYBACK_SETUP, this, &PlaybackController::openPlaybackSetupDialog);
    dispatcher()->reg(this, TOGGLE_HEAR_PLAYBACK_WHEN_EDITING_CODE, this, &PlaybackController::toggleHearPlaybackWhenEditing);
    dispatcher()->reg(this, "playback-reload-cache", this, &PlaybackController::reloadPlaybackCache);
    // KitNote: the recording of the score - attached, its offset, taken away (RecordingDialog.qml)
    dispatcher()->reg(this, "kitnote-recording", [this]() {
        interactive()->open("kitnote://playback/recording");
    });

    m_onlineSoundsController->regActions();

    globalContext()->currentNotationChanged().onNotify(this, [this]() {
        onNotationChanged();
    });

    globalContext()->currentProjectChanged().onNotify(this, [this]() {
        if (m_currentSequenceId != -1) {
            resetCurrentSequence();
        }

        if (!globalContext()->currentProject()) {
            return;
        }

        m_loadingProgress.start();

        playback()->addSequence().onResolve(this, [this](const TrackSequenceId& sequenceId) {
            setupNewCurrentSequence(sequenceId);
        });
    });

    m_totalPlayTimeChanged.onNotify(this, [this]() {
        updateCurrentTempo();

        updateLoop();
    });

    notationConfiguration()->isMidiInputEnabledChanged().onNotify(this, [this]() {
        notifyActionCheckedChanged(MIDI_ON_CODE);
    });

    configuration()->playNotesWhenEditingChanged().onNotify(this, [this]() {
        notifyActionCheckedChanged(TOGGLE_HEAR_PLAYBACK_WHEN_EDITING_CODE);
    });

    m_measureInputLag = configuration()->shouldMeasureInputLag();
}

void PlaybackController::updateCurrentTempo()
{
    if (!notationPlayback()) {
        return;
    }

    const Tempo& newTempo = notationPlayback()->multipliedTempo(m_currentTick);

    if (newTempo == m_currentTempo) {
        return;
    }

    m_currentTempo = newTempo;
    m_currentTempoChanged.notify();
}

bool PlaybackController::isPlayAllowed() const
{
    bool allowed = m_currentSequenceId != -1 && m_notation != nullptr && m_notation->hasVisibleParts() && isLoaded();
    return allowed;
}

Notification PlaybackController::isPlayAllowedChanged() const
{
    return m_isPlayAllowedChanged;
}

bool PlaybackController::isPlaying() const
{
    if (!currentPlayer()) {
        return false;
    }
    return currentPlayer()->playbackStatus() == PlaybackStatus::Running;
}

bool PlaybackController::isPaused() const
{
    if (!currentPlayer()) {
        return false;
    }
    return currentPlayer()->playbackStatus() == PlaybackStatus::Paused;
}

bool PlaybackController::isLoaded() const
{
    return m_loadingTrackCount == 0;
}

bool PlaybackController::isLoopEnabled() const
{
    return notationPlayback() && notationPlayback()->isLoopEnabled();
}

bool PlaybackController::loopBoundariesSet() const
{
    return notationPlayback() && !notationPlayback()->loopBoundaries().isNull();
}

Notification PlaybackController::isPlayingChanged() const
{
    return m_isPlayingChanged;
}

void PlaybackController::reset()
{
    stop();
}

void PlaybackController::seekRawTick(const midi::tick_t tick, const bool flushSound)
{
    if (m_currentTick == tick) {
        return;
    }

    RetVal<midi::tick_t> playedTick = notationPlayback()->playPositionTickByRawTick(tick);
    if (!playedTick.ret) {
        return;
    }

    seek(playedTickToSecs(playedTick.val), flushSound);
}

void PlaybackController::seek(const audio::secs_t secs, const bool flushSound)
{
    IF_ASSERT_FAILED(currentPlayer()) {
        return;
    }

    currentPlayer()->seek(secs, flushSound);
}

kfw::async::Channel<secs_t, tick_t> PlaybackController::currentPlaybackPositionChanged() const
{
    return m_currentPlaybackPositionChanged;
}

TrackSequenceId PlaybackController::currentTrackSequenceId() const
{
    return m_currentSequenceId;
}

Notification PlaybackController::currentTrackSequenceIdChanged() const
{
    return m_currentSequenceIdChanged;
}

const IPlaybackController::InstrumentTrackIdMap& PlaybackController::instrumentTrackIdMap() const
{
    return m_instrumentTrackIdMap;
}

const IPlaybackController::AuxTrackIdMap& PlaybackController::auxTrackIdMap() const
{
    return m_auxTrackIdMap;
}

Channel<TrackId> PlaybackController::trackAdded() const
{
    return m_trackAdded;
}

Channel<TrackId> PlaybackController::trackRemoved() const
{
    return m_trackRemoved;
}

std::string PlaybackController::auxChannelName(aux_channel_idx_t index) const
{
    return resolveAuxTrackTitle(index, audioSettings()->auxOutputParams(index));
}

Channel<aux_channel_idx_t, std::string> PlaybackController::auxChannelNameChanged() const
{
    return m_auxChannelNameChanged;
}

Promise<SoundPresetList> PlaybackController::availableSoundPresets(const InstrumentTrackId& instrumentTrackId) const
{
    auto it = m_instrumentTrackIdMap.find(instrumentTrackId);
    if (it == m_instrumentTrackIdMap.end()) {
        return Promise<SoundPresetList>([](auto, auto reject) {
            return reject(static_cast<int>(Ret::Code::UnknownError), "invalid instrumentTrackId");
        });
    }

    const AudioInputParams& params = audioSettings()->trackInputParams(instrumentTrackId);
    return playback()->availableSoundPresets(params.resourceMeta);
}

const PlaybackController::SoloMuteState& PlaybackController::trackSoloMuteState(const InstrumentTrackId& trackId) const
{
    return m_notation->soloMuteState()->trackSoloMuteState(trackId);
}

void PlaybackController::setTrackSoloMuteState(const InstrumentTrackId& trackId, const SoloMuteState& state)
{
    if (trackId == notationPlayback()->metronomeTrackId()) {
        if (state.mute != notationConfiguration()->isMetronomeEnabled()) {
            return;
        }

        toggleMetronome();
        return;
    }

    m_notation->soloMuteState()->setTrackSoloMuteState(trackId, state);
}

void PlaybackController::playElements(const std::vector<const notation::EngravingItem*>& elements, const PlayParams& params, bool isMidi)
{
    IF_ASSERT_FAILED(notationPlayback()) {
        return;
    }

    if ((!configuration()->playNotesWhenEditing()) || (isMidi && !configuration()->playNotesOnMidiInput())) {
        return;
    }

    if (m_measureInputLag) {
        START_INPUT_LAG_TIMER;
    }

    std::vector<const notation::EngravingItem*> elementsForPlaying;
    elementsForPlaying.reserve(elements.size());

    bool playChordWhenEditing = configuration()->playChordWhenEditing();
    bool playHarmonyWhenEditing = configuration()->playHarmonyWhenEditing();

    for (const EngravingItem* element : elements) {
        IF_ASSERT_FAILED(element) {
            continue;
        }

        if (!element->isPlayable()) {
            continue;
        }

        if (element->isChord() && !playChordWhenEditing) {
            continue;
        }

        if (element->isHarmony() && !playHarmonyWhenEditing) {
            continue;
        }

        elementsForPlaying.push_back(element);
    }

    const mpe::duration_t duration = params.duration.has_value() ? params.duration.value()
                                     : notationConfiguration()->notePlayDurationMilliseconds() * 1000;

    notationPlayback()->triggerEventsForItems(elementsForPlaying, duration, params.flushSound);
}

void PlaybackController::playNotes(const NoteValList& notes, staff_idx_t staffIdx, const Segment* segment, const PlayParams& params)
{
    Segment* seg = const_cast<Segment*>(segment);
    Chord* chord = engraving::Factory::createChord(seg);
    chord->setParent(seg);

    std::vector<const EngravingItem*> elements;
    elements.reserve(notes.size());

    for (const NoteVal& nval : notes) {
        Note* note = engraving::Factory::createNote(chord);
        note->setParent(chord);
        note->setStaffIdx(staffIdx);
        note->setNval(nval);
        elements.push_back(note);
    }

    playElements(elements, params);

    delete chord;
    DeleteAll(elements);
}

void PlaybackController::playMetronome(int tick)
{
    notationPlayback()->triggerMetronome(tick);
}

void PlaybackController::triggerControllers(const kfw::mpe::ControllerChangeEventList& list, staff_idx_t staffIdx, int tick)
{
    notationPlayback()->triggerControllers(list, staffIdx, tick);
}

void PlaybackController::seekElement(const notation::EngravingItem* element, bool flushSound)
{
    IF_ASSERT_FAILED(notationPlayback()) {
        return;
    }

    if (!element) {
        return;
    }

    RetVal<midi::tick_t> tick = notationPlayback()->playPositionTickByElement(element);
    if (!tick.ret) {
        return;
    }

    seek(playedTickToSecs(tick.val), flushSound);
}

void PlaybackController::seekBeat(int measureIndex, int beatIndex, bool flushSound)
{
    secs_t targetSecs = beatToSecs(measureIndex, beatIndex);
    seek(targetSecs, flushSound);
}

void PlaybackController::seekRangeSelection()
{
    if (!selection()->isRange()) {
        return;
    }

    midi::tick_t startTick = selectionRange()->startTick().ticks();

    seekRawTick(startTick);
}

void PlaybackController::onAudioResourceChanged(const TrackId trackId, const InstrumentTrackId& instrumentTrackId,
                                                const AudioResourceMeta& oldMeta, const AudioResourceMeta& newMeta)
{
    INotationPlaybackPtr notationPlayback = this->notationPlayback();
    if (!notationPlayback) {
        return;
    }

    if (oldMeta.type == newMeta.type && oldMeta.id == newMeta.id) {
        return;
    }

    if (shouldLoadDrumset(instrumentTrackId, oldMeta, newMeta)) {
        m_drumsetLoader.loadDrumset(m_notation, instrumentTrackId, newMeta);
    }

    notationPlayback->removeSoundFlags({ instrumentTrackId });

    if (audio::isOnlineAudioResource(newMeta)) {
        m_onlineSoundsController->addOnlineTrack(trackId, newMeta);
        tours()->onEvent(u"online_sounds_added");
        notationPlayback->setSendEventsOnScoreChange(instrumentTrackId, true);
    } else if (audio::isOnlineAudioResource(oldMeta)) {
        m_onlineSoundsController->removeOnlineTrack(trackId);
        notationPlayback->setSendEventsOnScoreChange(instrumentTrackId, false);
    }
}

bool PlaybackController::shouldLoadDrumset(const engraving::InstrumentTrackId& instrumentTrackId,
                                           const AudioResourceMeta& oldMeta, const AudioResourceMeta& newMeta) const
{
    if (oldMeta.type == newMeta.type && oldMeta.id == newMeta.id) {
        return false;
    }

    const Part* part = masterNotationParts()->part(instrumentTrackId.partId);
    const Instrument* instrument = part ? part->instrumentById(instrumentTrackId.instrumentId) : nullptr;
    if (!instrument || !instrument->useDrumset()) {
        return false;
    }

    return oldMeta.type == AudioResourceType::MuseSamplerSoundPack || newMeta.type == AudioResourceType::MuseSamplerSoundPack;
}

void PlaybackController::addSoundFlagsIfNeed(const std::vector<EngravingItem*>& selection)
{
    if (selection.empty()) {
        return;
    }

    std::vector<StaffText*> staffTextList;

    for (EngravingItem* item : selection) {
        if (!item || !item->isStaffText()) {
            continue;
        }

        InstrumentTrackId trackId = kn::engraving::makeInstrumentTrackId(item);
        const AudioInputParams& params = audioSettings()->trackInputParams(trackId);

        bool supportsSoundFlags = params.type() == AudioSourceType::MuseSampler;
        if (supportsSoundFlags) {
            staffTextList.push_back(toStaffText(item));
        }
    }

    if (!staffTextList.empty()) {
        notationPlayback()->addSoundFlags(staffTextList);
    }
}

kfw::audio::IPlayerPtr PlaybackController::currentPlayer() const
{
    return m_player;
}

INotationPlaybackPtr PlaybackController::notationPlayback() const
{
    return m_masterNotation ? m_masterNotation->playback() : nullptr;
}

INotationPartsPtr PlaybackController::masterNotationParts() const
{
    return m_masterNotation ? m_masterNotation->parts() : nullptr;
}

INotationSelectionPtr PlaybackController::selection() const
{
    return m_notation ? m_notation->interaction()->selection() : nullptr;
}

INotationSelectionRangePtr PlaybackController::selectionRange() const
{
    INotationSelectionPtr selection = this->selection();
    return selection ? selection->range() : nullptr;
}

INotationInteractionPtr PlaybackController::interaction() const
{
    return m_notation ? m_notation->interaction() : nullptr;
}

uint64_t PlaybackController::notationPlaybackKey() const
{
    return reinterpret_cast<uint64_t>(notationPlayback().get());
}

void PlaybackController::onNotationChanged()
{
    setNotation(globalContext()->currentNotation());
}

void PlaybackController::onPartChanged(const Part* part)
{
    if (!m_notation->hasVisibleParts()) {
        pause();
    }
    m_isPlayAllowedChanged.notify();

    if (!configuration()->muteHiddenInstruments()) {
        return;
    }

    for (const InstrumentTrackId& instrumentTrackId : part->instrumentTrackIdList()) {
        auto soloMuteState = trackSoloMuteState(instrumentTrackId);
        soloMuteState.mute = !part->show();
        setTrackSoloMuteState(instrumentTrackId, soloMuteState);
    }

    if (part->hasChordSymbol()) {
        InstrumentTrackId chordSymbolsTrackId = notationPlayback()->chordSymbolsTrackId(part->id());
        auto chordsSoloMuteState = trackSoloMuteState(chordSymbolsTrackId);
        chordsSoloMuteState.mute = !part->show();
        setTrackSoloMuteState(chordSymbolsTrackId, chordsSoloMuteState);
    }
}

void PlaybackController::onSelectionChanged()
{
    const INotationSelectionPtr selection = this->selection();
    if (!selection || !m_player) {
        return;
    }

    bool selectionTypeChanged = m_isRangeSelection && !selection->isRange();
    m_isRangeSelection = selection->isRange();

    if (!m_isRangeSelection) {
        if (selectionTypeChanged) {
            updateLoop();
            updateSoloMuteStates();
        }

        addSoundFlagsIfNeed(selection->elements());
        return;
    }

    m_player->resetLoop();

    seekRangeSelection();
    updateSoloMuteStates();
}

void PlaybackController::togglePlay(bool showErrors)
{
    if (!isPlayAllowed()) {
        LOGW() << "playback not allowed";
        return;
    }

    if (showErrors && m_onlineSoundsController->shouldShowOnlineSoundsProcessingError(isPlaying())) {
        m_onlineSoundsController->showOnlineSoundsProcessingError([this]() { togglePlay(false /*showErrors*/); });
        return;
    }

    interaction()->endEditElement();

    if (!isPlaying()) {
        recheckRecording();     // KitNote: a recording that was missing may be there now
    }

    if (isPlaying()) {
        pause();
    } else if (isPaused()) {
        notationPlayback()->sendEventsForChangedTracks();

        if (currentPlayer()) {
            secs_t pos = currentPlayer()->playbackPosition();
            secs_t endSecs = totalPlayTime();
            if (pos == endSecs) {
                secs_t startSecs = playbackStartSecs();
                seek(startSecs);
            }

            resume();
        }
    } else {
        notationPlayback()->sendEventsForChangedTracks();

        play();
    }
}

void PlaybackController::playFromSelection(bool showErrors)
{
    recheckRecording();     // KitNote: a recording that was missing may be there now

    if (showErrors && m_onlineSoundsController->shouldShowOnlineSoundsProcessingError(isPlaying())) {
        m_onlineSoundsController->showOnlineSoundsProcessingError([this]() { playFromSelection(false /*showErrors*/); });
        return;
    }

    int startTick = 0;

    if (!selection()->isNone()) {
        startTick = INT_MAX;
        for (const EngravingItem* item : selection()->elements()) {
            startTick = std::min(startTick, item->tick().ticks());
        }
    } else {
        // Selection is none - fall back to last element hit...
        if (const EngravingItem* lastElementHit = selection()->lastElementHit()) {
            startTick = lastElementHit->tick().ticks();
        }
    }

    const LoopBoundaries& loop = notationPlayback()->loopBoundaries();
    if (loop.enabled) {
        if (startTick < loop.loopInTick.ticks() || startTick > loop.loopOutTick.ticks()) {
            startTick = loop.loopInTick.ticks();
        }
    }

    const RetVal<midi::tick_t> retval = notationPlayback()->playPositionTickByRawTick(startTick);
    if (!retval.ret) {
        return;
    }

    seek(playedTickToSecs(retval.val));

    if (isPaused()) {
        resume();
    } else if (!isPlaying()) {
        play();
    }
}

void PlaybackController::play()
{
    IF_ASSERT_FAILED(currentPlayer()) {
        return;
    }

    if (isLoopEnabled()) {
        secs_t startSecs = playbackStartSecs();
        seek(startSecs);
    }

    currentPlayer()->prepareToPlay().onResolve(this, [this](const Ret& ret) {
        if (!currentPlayer()) {
            return;
        }

        if (!ret) {
            LOGE() << ret.toString();
        }

        secs_t delay = 0.;
        if (notationConfiguration()->isCountInEnabled()) {
            notationPlayback()->triggerCountIn(m_currentTick, delay);
        }

        currentPlayer()->play(delay);
    });
}

void PlaybackController::rewind(const ActionData& args)
{
    secs_t startSecs = playbackStartSecs();
    secs_t endSecs = totalPlayTime();
    secs_t newPosition = !args.empty() ? args.arg<secs_t>(0) : secs_t{ 0 };
    newPosition = std::clamp(newPosition, startSecs, endSecs);

    seek(newPosition);
}

void PlaybackController::pause(bool select)
{
    IF_ASSERT_FAILED(currentPlayer()) {
        return;
    }

    if (isPaused()) {
        return;
    }

    currentPlayer()->pause();

    if (select && m_notation) {
        const Fraction playPositionFrac = Fraction::fromTicks(m_currentTick);
        interaction()->findAndSelectChordRest(playPositionFrac);
    }
}

void PlaybackController::stop()
{
    IF_ASSERT_FAILED(currentPlayer()) {
        return;
    }

    currentPlayer()->stop();
}

void PlaybackController::resume()
{
    IF_ASSERT_FAILED(currentPlayer()) {
        return;
    }

    currentPlayer()->prepareToPlay().onResolve(this, [this](const Ret& ret) {
        if (!currentPlayer()) {
            return;
        }

        if (!ret) {
            LOGE() << ret.toString();
        }

        secs_t delay = 0.;
        if (notationConfiguration()->isCountInEnabled()) {
            notationPlayback()->triggerCountIn(m_currentTick, delay);
        }

        currentPlayer()->resume(delay);
    });
}

void PlaybackController::onPlaybackStatusChanged()
{
    if (!notationPlayback()) {
        return;
    }

    bool playing = isPlaying();
    const auto& onlineSounds = m_onlineSoundsController->onlineSounds();

    for (const auto& pair : m_instrumentTrackIdMap) {
        if (pair.first == recordingTrackId()) {
            continue;       // KitNote: not a track of the score's notes
        }
        bool shouldSendOnScoreChange = playing || kfw::contains(onlineSounds, pair.second);
        notationPlayback()->setSendEventsOnScoreChange(pair.first, shouldSendOnScoreChange);
    }
}

secs_t PlaybackController::playbackStartSecs() const
{
    if (!m_notation) {
        return 0;
    }

    const LoopBoundaries& loop = notationPlayback()->loopBoundaries();
    if (loop.enabled) {
        // Convert from raw ticks (visual tick != playback tick due to repeats etc)
        RetVal<tick_t> startTick = notationPlayback()->playPositionTickByRawTick(loop.loopInTick.ticks());
        if (!startTick.ret) {
            return 0;
        }
        return playedTickToSecs(startTick.val);
    }

    return 0;
}

InstrumentTrackIdSet PlaybackController::instrumentTrackIdSetForRangePlayback() const
{
    std::vector<const Part*> selectedParts = selectionRange()->selectedParts();
    Fraction startTick = selectionRange()->startTick();
    int startTicks = startTick.ticks();

    InstrumentTrackIdSet result;

    for (const Part* part : selectedParts) {
        if (const Instrument* startInstrument = part->instrument(startTick)) {
            result.insert({ part->id(), startInstrument->id() });
        }

        for (auto [tick, instrument] : part->instruments()) {
            if (tick > startTicks) {
                result.insert({ part->id(), instrument->id() });
            }
        }

        if (part->hasChordSymbol()) {
            result.insert(notationPlayback()->chordSymbolsTrackId(part->id()));
        }
    }

    return result;
}

void PlaybackController::togglePlayRepeats()
{
    bool playRepeatsEnabled = notationConfiguration()->isPlayRepeatsEnabled();
    notationConfiguration()->setIsPlayRepeatsEnabled(!playRepeatsEnabled);
    notifyActionCheckedChanged(REPEAT_CODE);
}

void PlaybackController::togglePlayChordSymbols()
{
    bool playChordSymbolsEnabled = notationConfiguration()->isPlayChordSymbolsEnabled();
    notationConfiguration()->setIsPlayChordSymbolsEnabled(!playChordSymbolsEnabled);
    notifyActionCheckedChanged(PLAY_CHORD_SYMBOLS_CODE);

    for (auto it = m_instrumentTrackIdMap.cbegin(); it != m_instrumentTrackIdMap.cend(); ++it) {
        if (notationPlayback()->isChordSymbolsTrack(it->first)) {
            setTrackActivity(it->first, !playChordSymbolsEnabled);
        }
    }
}

void PlaybackController::toggleAutomaticallyPan()
{
    bool panEnabled = notationConfiguration()->isAutomaticallyPanEnabled();
    notationConfiguration()->setIsAutomaticallyPanEnabled(!panEnabled);
    notifyActionCheckedChanged(PAN_CODE);
}

void PlaybackController::toggleMetronome()
{
    bool metronomeEnabled = notationConfiguration()->isMetronomeEnabled();
    bool countInEnabled = notationConfiguration()->isCountInEnabled();

    notationConfiguration()->setIsMetronomeEnabled(!metronomeEnabled);
    notifyActionCheckedChanged(METRONOME_CODE);

    setTrackActivity(notationPlayback()->metronomeTrackId(), !metronomeEnabled || countInEnabled);
}

void PlaybackController::toggleCountIn()
{
    bool metronomeEnabled = notationConfiguration()->isMetronomeEnabled();
    bool countInEnabled = notationConfiguration()->isCountInEnabled();

    notationConfiguration()->setIsCountInEnabled(!countInEnabled);
    notifyActionCheckedChanged(COUNT_IN_CODE);

    setTrackActivity(notationPlayback()->metronomeTrackId(), metronomeEnabled || !countInEnabled);
}

void PlaybackController::toggleMidiInput()
{
    bool wasMidiInputEnabled = notationConfiguration()->isMidiInputEnabled();
    notationConfiguration()->setIsMidiInputEnabled(!wasMidiInputEnabled);
}

void PlaybackController::setMidiUseWrittenPitch(bool useWrittenPitch)
{
    notationConfiguration()->setMidiUseWrittenPitch(useWrittenPitch);
    notifyActionCheckedChanged(INPUT_WRITTEN_PITCH);
    notifyActionCheckedChanged(INPUT_SOUNDING_PITCH);
}

void PlaybackController::toggleLoopPlayback()
{
    if (isLoopEnabled()) {
        disableLoop();
        return;
    }

    if (loopBoundariesSet() && !selection()->isRange()) {
        enableLoop();
        return;
    }

    int loopInTick = 0;
    int loopOutTick = 0;

    if (!selection()->isNone()) {
        loopInTick = selectionRange()->startTick().ticks();
        loopOutTick = selectionRange()->endTick().ticks();
    }

    if (loopInTick <= 0) {
        loopInTick = INotationPlayback::FirstScoreTick;
    }

    if (loopOutTick <= 0) {
        loopOutTick = INotationPlayback::LastScoreTick;
    }

    addLoopBoundaryToTick(LoopBoundaryType::LoopIn, loopInTick);
    addLoopBoundaryToTick(LoopBoundaryType::LoopOut, loopOutTick);
}

void PlaybackController::toggleHearPlaybackWhenEditing()
{
    bool wasPlayNotesWhenEditing = configuration()->playNotesWhenEditing();
    configuration()->setPlayNotesWhenEditing(!wasPlayNotesWhenEditing);
}

void PlaybackController::reloadPlaybackCache()
{
    INotationPlaybackPtr nPlayback = notationPlayback();
    if (nPlayback) {
        nPlayback->reload();
    }
}

void PlaybackController::openPlaybackSetupDialog()
{
    interactive()->open("kitnote://playback/soundprofilesdialog");
}

void PlaybackController::addLoopBoundary(LoopBoundaryType type)
{
    if (isPlaying()) {
        addLoopBoundaryToTick(type, m_currentTick);
    } else {
        addLoopBoundaryToTick(type, INotationPlayback::SelectedNoteTick);
    }
}

void PlaybackController::addLoopBoundaryToTick(LoopBoundaryType type, int tick)
{
    if (notationPlayback()) {
        notationPlayback()->addLoopBoundary(type, tick);
        enableLoop();
    }
}

void PlaybackController::updateLoop()
{
    if (!notationPlayback() || !currentPlayer()) {
        return;
    }

    const LoopBoundaries& boundaries = notationPlayback()->loopBoundaries();

    if (!boundaries.enabled) {
        disableLoop();
        return;
    }

    // Convert from raw ticks (visual tick != playback tick due to repeats etc)
    RetVal<tick_t> playbackTickFrom = notationPlayback()->playPositionTickByRawTick(boundaries.loopInTick.ticks());
    RetVal<tick_t> playbackTickTo = notationPlayback()->playPositionTickByRawTick(boundaries.loopOutTick.ticks());
    if (!playbackTickFrom.ret || !playbackTickTo.ret) {
        return;
    }

    secs_t fromSecs = playedTickToSecs(playbackTickFrom.val);
    secs_t toSecs = playedTickToSecs(playbackTickTo.val);
    currentPlayer()->setLoop(secsToMilisecs(fromSecs), secsToMilisecs(toSecs));

    enableLoop();

    notifyActionCheckedChanged(LOOP_CODE);
}

void PlaybackController::enableLoop()
{
    if (notationPlayback()) {
        notationPlayback()->setLoopBoundariesEnabled(true);
    }
}

void PlaybackController::disableLoop()
{
    IF_ASSERT_FAILED(notationPlayback() && currentPlayer()) {
        return;
    }

    currentPlayer()->resetLoop();
    notationPlayback()->setLoopBoundariesEnabled(false);

    notifyActionCheckedChanged(LOOP_CODE);
}

void PlaybackController::notifyActionCheckedChanged(const ActionCode& actionCode)
{
    m_actionCheckedChanged.send(actionCode);
}

kn::project::IProjectAudioSettingsPtr PlaybackController::audioSettings() const
{
    IF_ASSERT_FAILED(globalContext()->currentProject()) {
        return nullptr;
    }

    return globalContext()->currentProject()->audioSettings();
}

void PlaybackController::resetCurrentSequence()
{
    if (currentPlayer()) {
        currentPlayer()->playbackPositionChanged().disconnect(this);
        currentPlayer()->playbackStatusChanged().disconnect(this);
    }

    playback()->clearSources();
    playback()->inputParamsChanged().disconnect(this);

    playback()->clearAllFx();
    playback()->outputParamsChanged().disconnect(this);
    playback()->masterOutputParamsChanged().disconnect(this);
    playback()->clearMasterOutputParams();

    m_seqAsyncReceiver.async_disconnectAll();

    m_currentTick = 0;

    playback()->removeSequence(m_currentSequenceId);

    m_instrumentTrackIdMap.clear();
    m_auxTrackIdMap.clear();

    // KitNote: the recording went with the sequence
    ++m_recordingRequest;
    m_recordingFile.clear();
    m_recordingStoredPath.clear();
    m_recordingScorePath.clear();
    m_recordingFound.clear();
    setRecordingState(RecordingState());

    m_isRangeSelection = false;

    m_currentSequenceId = -1;
    m_currentSequenceIdChanged.notify();

    m_player = nullptr;
    globalContext()->setCurrentPlayer(nullptr);

    m_onlineSoundsController->resetCurrentSequence();
}

void PlaybackController::addTrack(const InstrumentTrackId& instrumentTrackId, const TrackAddFinished& onFinished)
{
    if (notationPlayback()->metronomeTrackId() == instrumentTrackId) {
        doAddTrack(instrumentTrackId, kfw::trc("playback", "Metronome"), onFinished);
        return;
    }

    const Part* part = masterNotationParts()->part(instrumentTrackId.partId);
    if (!part) {
        return;
    }

    if (notationPlayback()->isChordSymbolsTrack(instrumentTrackId)) {
        const std::string trackName = kfw::trc("playback", "Chords") + "." + part->partName().toStdString();
        doAddTrack(instrumentTrackId, trackName, onFinished);
        return;
    }

    const kfw::String primaryInstrId = part->instrument()->id();
    if (instrumentTrackId.instrumentId == primaryInstrId) {
        const std::string trackName = part->partName().toStdString();
        doAddTrack(instrumentTrackId, trackName, onFinished);
        return;
    }

    const Instrument* instrument = part->instrumentById(instrumentTrackId.instrumentId);
    if (instrument != nullptr) {
        std::string trackName = "(" + instrument->trackName().toStdString() + ")";
        doAddTrack(instrumentTrackId, trackName, onFinished);
    }
}

void PlaybackController::doAddTrack(const InstrumentTrackId& instrumentTrackId, const std::string& title,
                                    const TrackAddFinished& onFinished)
{
    IF_ASSERT_FAILED(notationPlayback() && playback()) {
        return;
    }

    if (!instrumentTrackId.isValid()) {
        return;
    }

    mpe::PlaybackData playbackData = notationPlayback()->trackPlaybackData(instrumentTrackId);
    if (!playbackData.isValid()) {
        return;
    }

    AudioInputParams inParams = audioSettings()->trackInputParams(instrumentTrackId);
    AudioOutputParams outParams = trackOutputParams(instrumentTrackId);
    AudioResourceMeta originMeta = inParams.resourceMeta;

    bool isMetronome = notationPlayback()->metronomeTrackId() == instrumentTrackId;

    if (!inParams.isValid()) {
        if (isMetronome) {
            const SoundProfile& profile = profilesRepo()->profile(configuration()->basicSoundProfileName());
            inParams = { profile.findResource(playbackData.setupData), {} };
        } else {
            const SoundProfile& profile = profilesRepo()->profile(audioSettings()->activeSoundProfile());
            inParams = { profile.findResource(playbackData.setupData), {} };
        }
    }

    if (!isMetronome && outParams.auxSends.empty()) {
        const kfw::String& instrumentSoundId = inParams.resourceMeta.attributeVal(PLAYBACK_SETUP_DATA_ATTRIBUTE);
        AudioSourceType sourceType = inParams.isValid() ? inParams.type() : AudioSourceType::Fluid;

        for (aux_channel_idx_t idx = 0; idx < AUX_CHANNEL_NUM; ++idx) {
            gain_t signalAmount = configuration()->defaultAuxSendValue(idx, sourceType, instrumentSoundId);
            outParams.auxSends.emplace_back(AuxSendParams { signalAmount, true });
        }
    }

    uint64_t playbackKey = notationPlaybackKey();

    playback()->addTrack(m_currentSequenceId, title, std::move(playbackData), { std::move(inParams), std::move(outParams) })
    .onResolve(this, [this, title, instrumentTrackId, playbackKey, onFinished, originMeta](const TrackId trackId,
                                                                                           const AudioParams& appliedParams) {
        //! NOTE It may be that while we were adding a track, the notation was already closed (or opened another)
        //! This situation can be if the notation was opened and immediately closed.
        if (notationPlaybackKey() != playbackKey) {
            return;
        }

        m_instrumentTrackIdMap.insert({ instrumentTrackId, trackId });

        const bool trackNewlyAdded = !audioSettings()->trackHasExistingOutputParams(instrumentTrackId);
        audioSettings()->setTrackInputParams(instrumentTrackId, appliedParams.in);
        audioSettings()->setTrackOutputParams(instrumentTrackId, appliedParams.out);

        updateSoloMuteStates();

        // KitNote: a metronome track that arrives after the export has begun was set up for normal playback.
        if (m_isExportingAudio && notationPlayback() && instrumentTrackId == notationPlayback()->metronomeTrackId()) {
            setTrackActivity(instrumentTrackId, exportAudioWithMetronome());
        }

        onFinished();

        m_trackAdded.send(trackId);

        if (trackNewlyAdded) {
            onTrackNewlyAdded(instrumentTrackId);
        }

        if (shouldLoadDrumset(instrumentTrackId, originMeta, appliedParams.in.resourceMeta)) {
            m_drumsetLoader.loadDrumset(m_notation, instrumentTrackId, appliedParams.in.resourceMeta);
        }

        if (kfw::audio::isOnlineAudioResource(appliedParams.in.resourceMeta)) {
            m_onlineSoundsController->addOnlineTrack(trackId, appliedParams.in.resourceMeta);

            if (notationPlayback()) {
                notationPlayback()->setSendEventsOnScoreChange(instrumentTrackId, true);
            }
        }
    })
    .onReject(this, [instrumentTrackId, onFinished](int code, const std::string& msg) {
        LOGE() << "can't add a new track, code: [" << code << "] " << msg;

        onFinished();
    });

    m_loadingTrackCount++;
}

void PlaybackController::addAuxTrack(aux_channel_idx_t index, const TrackAddFinished& onFinished)
{
    IF_ASSERT_FAILED(notationPlayback() && playback()) {
        return;
    }

    AudioOutputParams outParams;

    if (audioSettings()->containsAuxOutputParams(index)) {
        outParams = audioSettings()->auxOutputParams(index);
    } else if (index == REVERB_CHANNEL_IDX) {
        outParams = makeReverbOutputParams();
    }

    std::string title = resolveAuxTrackTitle(index, outParams, false);
    uint64_t playbackKey = notationPlaybackKey();

    playback()->addAuxTrack(m_currentSequenceId, title, outParams)
    .onResolve(this, [this, playbackKey, index, onFinished](const TrackId trackId, const AudioOutputParams& appliedParams) {
        //! NOTE It may be that while we were adding a track, the notation was already closed (or opened another)
        //! This situation can be if the notation was opened and immediately closed.
        if (notationPlaybackKey() != playbackKey) {
            return;
        }

        m_auxTrackIdMap.insert({ index, trackId });

        audioSettings()->setAuxOutputParams(index, appliedParams);

        updateSoloMuteStates();
        onFinished();

        m_trackAdded.send(trackId);
    })
    .onReject(this, [onFinished](int code, const std::string& msg) {
        LOGE() << "can't add a new aux track, code: [" << code << "] " << msg;

        onFinished();
    });

    m_loadingTrackCount++;
}

void PlaybackController::setTrackActivity(const engraving::InstrumentTrackId& instrumentTrackId, const bool isActive)
{
    IF_ASSERT_FAILED(audioSettings() && playback()) {
        return;
    }

    AudioOutputParams outParams = audioSettings()->trackOutputParams(instrumentTrackId);

    outParams.muted = !isActive;

    audio::TrackId trackId = m_instrumentTrackIdMap[instrumentTrackId];
    playback()->setOutputParams(m_currentSequenceId, trackId, std::move(outParams));
}

AudioOutputParams PlaybackController::trackOutputParams(const InstrumentTrackId& instrumentTrackId) const
{
    IF_ASSERT_FAILED(audioSettings() && notationConfiguration() && notationPlayback()) {
        return {};
    }

    AudioOutputParams result = audioSettings()->trackOutputParams(instrumentTrackId);

    if (instrumentTrackId == notationPlayback()->metronomeTrackId()) {
        if (m_isExportingAudio) {
            // KitNote: the export dialog decides, not the metronome button.
            result.muted = !exportAudioWithMetronome();
        } else {
            result.muted = !notationConfiguration()->isMetronomeEnabled() && !notationConfiguration()->isCountInEnabled();
        }
        return result;
    }

    if (notationPlayback()->isChordSymbolsTrack(instrumentTrackId)) {
        result.muted = !notationConfiguration()->isPlayChordSymbolsEnabled();
    }

    return result;
}

void PlaybackController::removeTrack(const InstrumentTrackId& instrumentTrackId)
{
    IF_ASSERT_FAILED(notationPlayback() && playback()) {
        return;
    }

    auto search = m_instrumentTrackIdMap.find(instrumentTrackId);

    if (search == m_instrumentTrackIdMap.end()) {
        return;
    }

    playback()->removeTrack(m_currentSequenceId, search->second);
    audioSettings()->removeTrackParams(instrumentTrackId);

    m_masterNotation->notation()->soloMuteState()->removeTrackSoloMuteState(instrumentTrackId);
    for (const IExcerptNotationPtr& excerpt : m_masterNotation->excerpts()) {
        if (const INotationPtr& notation = excerpt->notation()) {
            notation->soloMuteState()->removeTrackSoloMuteState(instrumentTrackId);
        }
    }

    m_onlineSoundsController->removeOnlineTrack(search->second);

    m_trackRemoved.send(search->second);
    m_instrumentTrackIdMap.erase(instrumentTrackId);
}

void PlaybackController::onTrackNewlyAdded(const InstrumentTrackId& instrumentTrackId)
{
    for (const IExcerptNotationPtr& excerpt : m_masterNotation->excerpts()) {
        if (const INotationPtr& notation = excerpt->notation()) {
            if (notation == m_notation || notation->soloMuteState()->trackSoloMuteStateExists(instrumentTrackId)) {
                continue;
            }

            const Part* part = notation->parts()->part(instrumentTrackId.partId);
            const bool shouldMute = !part || !part->show();

            const INotationSoloMuteState::SoloMuteState soloMuteState = { shouldMute, /*solo*/ false };
            notation->soloMuteState()->setTrackSoloMuteState(instrumentTrackId, soloMuteState);
        }
    }
}

void PlaybackController::setupNewCurrentSequence(const TrackSequenceId sequenceId)
{
    playback()->removeAllTracks(m_currentSequenceId);

    m_currentSequenceId = sequenceId;
    m_onlineSoundsController->setCurrentSequence(sequenceId);
    m_player = playback()->player(sequenceId);
    globalContext()->setCurrentPlayer(m_player);

    if (!notationPlayback()) {
        return;
    }

    const audio::AudioOutputParams& masterOutputParams = audioSettings()->masterAudioOutputParams();
    playback()->setMasterOutputParams(masterOutputParams);

    subscribeOnAudioParamsChanges();
    setupSequenceTracks();
    setupSequencePlayer();

    // KitNote: the score's recording; a score saved elsewhere may find it by another path
    updateRecordingTrack();
    if (globalContext()->currentProject()) {
        globalContext()->currentProject()->pathChanged().onNotify(this, [this]() {
            updateRecordingTrack();
        }, Asyncable::Mode::SetReplace);
    }

    m_currentSequenceIdChanged.notify();
}

void PlaybackController::subscribeOnAudioParamsChanges()
{
    playback()->masterOutputParamsChanged().onReceive(this, [this](const audio::AudioOutputParams& params) {
        audioSettings()->setMasterAudioOutputParams(params);
    });

    playback()->inputParamsChanged().onReceive(this,
                                               [this](const TrackSequenceId sequenceId,
                                                      const TrackId trackId,
                                                      const AudioInputParams& params) {
        if (sequenceId != m_currentSequenceId) {
            return;
        }

        auto search = std::find_if(m_instrumentTrackIdMap.begin(), m_instrumentTrackIdMap.end(), [trackId](const auto& pair) {
            return pair.second == trackId;
        });

        if (search != m_instrumentTrackIdMap.end() && search->first == recordingTrackId()) {
            onRecordingInputParamsChanged(params);      // KitNote: no sound to choose, only the file read
            return;
        }

        if (search != m_instrumentTrackIdMap.end()) {
            const AudioResourceMeta& oldMeta = audioSettings()->trackInputParams(search->first).resourceMeta;
            onAudioResourceChanged(trackId, search->first, oldMeta, params.resourceMeta);

            audioSettings()->setTrackInputParams(search->first, params);
        }
    });

    playback()->outputParamsChanged().onReceive(this,
                                                [this](const TrackSequenceId sequenceId,
                                                       const TrackId trackId,
                                                       const AudioOutputParams& params) {
        if (sequenceId != m_currentSequenceId) {
            return;
        }

        auto instrumentIt = std::find_if(m_instrumentTrackIdMap.begin(), m_instrumentTrackIdMap.end(), [trackId](const auto& pair) {
            return pair.second == trackId;
        });

        if (instrumentIt != m_instrumentTrackIdMap.end()) {
            audioSettings()->setTrackOutputParams(instrumentIt->first, params);
            return;
        }

        auto auxIt = std::find_if(m_auxTrackIdMap.begin(), m_auxTrackIdMap.end(), [trackId](const auto& pair) {
            return pair.second == trackId;
        });

        if (auxIt != m_auxTrackIdMap.end()) {
            aux_channel_idx_t auxIdx = auxIt->first;
            std::string oldName = resolveAuxTrackTitle(auxIdx, audioSettings()->auxOutputParams(auxIdx));
            std::string newName = resolveAuxTrackTitle(auxIdx, params);

            audioSettings()->setAuxOutputParams(auxIdx, params);

            if (oldName != newName) {
                m_auxChannelNameChanged.send(auxIdx, newName);
            }
        }
    });
}

void PlaybackController::setupSequenceTracks()
{
    m_instrumentTrackIdMap.clear();

    if (!masterNotationParts()) {
        return;
    }

    m_loadingTrackCount = 0;

    InstrumentTrackIdSet trackIdSet = notationPlayback()->existingTrackIdSet();
    size_t trackCount = trackIdSet.size() + AUX_CHANNEL_NUM;
    std::string title = kfw::trc("playback", "Loading audio samples");

    auto onAddFinished = [this, trackCount, title]() {
        m_loadingTrackCount--;

        size_t current = trackCount - m_loadingTrackCount;
        m_loadingProgress.progress(current, trackCount, title);

        if (m_loadingTrackCount == 0) {
            m_loadingProgress.finish(kfw::make_ok());
            m_isPlayAllowedChanged.notify();
        }
    };

    for (const InstrumentTrackId& trackId : trackIdSet) {
        addTrack(trackId, onAddFinished);
    }

    for (aux_channel_idx_t idx = 0; idx < AUX_CHANNEL_NUM; ++idx) {
        addAuxTrack(idx, onAddFinished);
    }

    m_loadingProgress.progress(0, trackCount, title);

    notationPlayback()->trackAdded().onReceive(this, [this, onAddFinished](const InstrumentTrackId& instrumentTrackId) {
        addTrack(instrumentTrackId, onAddFinished);
    });

    notationPlayback()->trackRemoved().onReceive(this, [this](const InstrumentTrackId& instrumentTrackId) {
        removeTrack(instrumentTrackId);
    });

    NotifyList<const Part*> partList = masterNotationParts()->partList();

    //! HACK - ideally we would use "this" (PlaybackController) instead of m_seqAsyncReceiver for the following
    //! subscription, but we've already subscribed to onItemChanged for a different reason in setNotation...
    partList.onItemChanged(&m_seqAsyncReceiver, [this](const Part*) {
        updateSoloMuteStates();
    });

    audioSettings()->auxSoloMuteStateChanged().onReceive(
        this, [this](aux_channel_idx_t, const notation::INotationSoloMuteState::SoloMuteState&) {
        updateSoloMuteStates();
    });

    m_isPlayAllowedChanged.notify();
}

void PlaybackController::setupSequencePlayer()
{
    currentPlayer()->playbackPositionChanged().onReceive(this, [this](const audio::secs_t pos) {
        if (m_isExportingAudio) {
            return;
        }

        m_currentTick = notationPlayback()->secToTick(pos);
        m_currentPlaybackPositionChanged.send(pos, m_currentTick);

        updateCurrentTempo();

        secs_t endSecs = totalPlayTime();
        if (pos + milisecsToSecs(1) >= endSecs) {
            stop();
        }
    });

    currentPlayer()->playbackStatusChanged().onReceive(this, [this](PlaybackStatus) {
        if (m_isExportingAudio) {
            return;
        }

        m_isPlayingChanged.notify();
        onPlaybackStatusChanged();
    });

    currentPlayer()->setDuration(secsToMilisecs(notationPlayback()->totalPlayTime()));

    notationPlayback()->totalPlayTimeChanged().onReceive(this, [this](const audio::secs_t totalPlaybackTime) {
        currentPlayer()->setDuration(secsToMilisecs(totalPlaybackTime));
        m_totalPlayTimeChanged.notify();
    });
}

void PlaybackController::updateSoloMuteStates()
{
    if (!audioSettings() || !playback() || !m_notation) {
        return;
    }

    TRACEFUNC;

    InstrumentTrackIdSet existingTrackIdSet = notationPlayback()->existingTrackIdSet();
    bool hasSolo = false;

    for (const InstrumentTrackId& instrumentTrackId : existingTrackIdSet) {
        if (instrumentTrackId == notationPlayback()->metronomeTrackId()) {
            continue;
        }
        if (m_notation->soloMuteState()->trackSoloMuteState(instrumentTrackId).solo) {
            hasSolo = true;
            break;
        }
    }

    // KitNote: the recording is soloed like an instrument (the recording on its own) - when it is heard:
    // not away from 100% (it is not time-stretched), and in an export only when that asks for it
    const bool hasRecording = kfw::contains(m_instrumentTrackIdMap, recordingTrackId());
    const bool recordingOffSpeed = !kfw::RealIsEqual(tempoMultiplier(), 1.0);
    const bool recordingAudible = !recordingOffSpeed && !(m_isExportingAudio && !exportAudioWithRecording());
    if (hasRecording && recordingAudible && m_notation->soloMuteState()->trackSoloMuteState(recordingTrackId()).solo) {
        hasSolo = true;
    }

    InstrumentTrackIdSet allowedInstrumentTrackIdSet = instrumentTrackIdSetForRangePlayback();
    bool isRangePlaybackMode = !m_isExportingAudio && selection()->isRange() && !allowedInstrumentTrackIdSet.empty();

    for (const InstrumentTrackId& instrumentTrackId : existingTrackIdSet) {
        if (!kfw::contains(m_instrumentTrackIdMap, instrumentTrackId)) {
            continue;
        }

        if (instrumentTrackId == notationPlayback()->metronomeTrackId()) {
            continue;
        }

        // 1. Recall the solo-mute state for this notation
        const auto& soloMuteState = m_notation->soloMuteState()->trackSoloMuteState(instrumentTrackId);

        // 2. Evaluate "force mute" (disabling the mute button)
        bool shouldForceMute = hasSolo && !soloMuteState.solo;
        if (notationPlayback()->isChordSymbolsTrack(instrumentTrackId) && !shouldForceMute) {
            shouldForceMute = !notationConfiguration()->isPlayChordSymbolsEnabled();
        }

        if (isRangePlaybackMode && !shouldForceMute) {
            shouldForceMute = !kfw::contains(allowedInstrumentTrackIdSet, instrumentTrackId);
        }

        // 3. Update params for playback / mixer
        AudioOutputParams params = trackOutputParams(instrumentTrackId);
        params.solo = soloMuteState.solo;
        params.muted = soloMuteState.mute || shouldForceMute;
        params.forceMute = shouldForceMute;

        audio::TrackId trackId = m_instrumentTrackIdMap.at(instrumentTrackId);
        playback()->setOutputParams(m_currentSequenceId, trackId, std::move(params));
    }

    if (hasRecording) {
        const auto& soloMuteState = m_notation->soloMuteState()->trackSoloMuteState(recordingTrackId());
        const bool shouldForceMute = !recordingAudible || (hasSolo && !soloMuteState.solo);

        AudioOutputParams params = trackOutputParams(recordingTrackId());
        params.solo = soloMuteState.solo;
        params.muted = soloMuteState.mute || shouldForceMute;
        params.forceMute = shouldForceMute;
        playback()->setOutputParams(m_currentSequenceId, m_instrumentTrackIdMap.at(recordingTrackId()), std::move(params));
    }

    if (m_recordingState.mutedBySpeed != (m_recordingState.attached && recordingOffSpeed)) {
        RecordingState state = m_recordingState;
        state.mutedBySpeed = state.attached && recordingOffSpeed;
        setRecordingState(state);
    }

    updateAuxMuteStates();
}

void PlaybackController::updateAuxMuteStates()
{
    for (const auto& pair : m_auxTrackIdMap) {
        auto soloMuteState = audioSettings()->auxSoloMuteState(pair.first);

        AudioOutputParams params = audioSettings()->auxOutputParams(pair.first);
        if (params.muted == soloMuteState.mute) {
            continue;
        }

        params.muted = soloMuteState.mute;
        playback()->setOutputParams(m_currentSequenceId, pair.second, std::move(params));
    }
}

bool PlaybackController::actionChecked(const ActionCode& actionCode) const
{
    QMap<std::string, bool> isChecked {
        { LOOP_CODE, isLoopEnabled() },
        { MIDI_ON_CODE, notationConfiguration()->isMidiInputEnabled() },
        { INPUT_WRITTEN_PITCH, notationConfiguration()->midiUseWrittenPitch().val },
        { INPUT_SOUNDING_PITCH, !notationConfiguration()->midiUseWrittenPitch().val },
        { REPEAT_CODE, notationConfiguration()->isPlayRepeatsEnabled() },
        { PLAY_CHORD_SYMBOLS_CODE, notationConfiguration()->isPlayChordSymbolsEnabled() },
        { PAN_CODE, notationConfiguration()->isAutomaticallyPanEnabled() },
        { METRONOME_CODE, notationConfiguration()->isMetronomeEnabled() },
        { COUNT_IN_CODE, notationConfiguration()->isCountInEnabled() },
        { TOGGLE_HEAR_PLAYBACK_WHEN_EDITING_CODE, configuration()->playNotesWhenEditing() }
    };

    return isChecked[actionCode];
}

Channel<ActionCode> PlaybackController::actionCheckedChanged() const
{
    return m_actionCheckedChanged;
}

secs_t PlaybackController::totalPlayTime() const
{
    auto np = notationPlayback();
    return np ? np->totalPlayTime() : secs_t { 0.0 };
}

Notification PlaybackController::totalPlayTimeChanged() const
{
    return m_totalPlayTimeChanged;
}

const Tempo& PlaybackController::currentTempo() const
{
    return m_currentTempo;
}

Notification PlaybackController::currentTempoChanged() const
{
    return m_currentTempoChanged;
}

MeasureBeat PlaybackController::currentBeat() const
{
    return notationPlayback() ? notationPlayback()->beat(m_currentTick) : MeasureBeat();
}

secs_t PlaybackController::beatToSecs(int measureIndex, int beatIndex) const
{
    if (!notationPlayback()) {
        return 0;
    }

    kfw::midi::tick_t rawTick = notationPlayback()->beatToRawTick(measureIndex, beatIndex);
    kfw::midi::tick_t playedTick = notationPlayback()->playPositionTickByRawTick(rawTick).val;

    return playedTickToSecs(playedTick);
}

double PlaybackController::tempoMultiplier() const
{
    return notationPlayback() ? notationPlayback()->tempoMultiplier() : 1.0;
}

void PlaybackController::setTempoMultiplier(double multiplier)
{
    if (!notationPlayback()) {
        return;
    }

    tick_t tick = m_currentTick;
    bool playing = isPlaying();

    if (playing) {
        pause();
    }

    notationPlayback()->setTempoMultiplier(multiplier);
    seekRawTick(tick);
    updateLoop();

    // KitNote: the recording is not time-stretched - away from 100% it is muted
    updateSoloMuteStates();

    if (playing) {
        resume();
    }
}

kfw::Progress PlaybackController::loadingProgress() const
{
    return m_loadingProgress;
}

void PlaybackController::applyProfile(const SoundProfileName& profileName)
{
    project::IProjectAudioSettingsPtr audioSettingsPtr = audioSettings();

    IF_ASSERT_FAILED(audioSettingsPtr) {
        return;
    }

    const SoundProfile& profile = profilesRepo()->profile(profileName);
    if (!profile.isValid()) {
        return;
    }

    notationPlayback()->removeSoundFlags(notationPlayback()->existingTrackIdSet());

    const InstrumentTrackId& metronomeTrackId = notationPlayback()->metronomeTrackId();

    for (const auto& pair : m_instrumentTrackIdMap) {
        if (pair.first == metronomeTrackId || pair.first == recordingTrackId()) {
            continue;
        }

        const mpe::PlaybackData& playbackData = notationPlayback()->trackPlaybackData(pair.first);
        AudioInputParams newInputParams { profile.findResource(playbackData.setupData), {} };

        playback()->setInputParams(m_currentSequenceId, pair.second, std::move(newInputParams));
    }

    audioSettingsPtr->setActiveSoundProfile(profileName);
}

void PlaybackController::setNotation(notation::INotationPtr notation)
{
    if (m_notation == notation) {
        return;
    }

    if (m_notation) {
        INotationPartsPtr notationParts = m_notation->parts();
        NotifyList<const Part*> partList = notationParts->partList();
        partList.disconnect(this);

        m_notation->interaction()->selectionChanged().disconnect(this);
        m_notation->interaction()->textEditingEnded().disconnect(this);
        m_notation->soloMuteState()->trackSoloMuteStateChanged().disconnect(this);
    }

    m_notation = notation;

    m_isPlayAllowedChanged.notify();

    if (!m_notation) {
        setMasterNotation(nullptr);
        return;
    }

    setMasterNotation(m_notation->masterNotation());

    if (!m_notation->hasVisibleParts()) {
        pause();
    }

    updateSoloMuteStates();

    NotifyList<const Part*> partList = m_notation->parts()->partList();

    partList.onItemAdded(this, [this](const Part* part) {
        onPartChanged(part);
    });

    partList.onItemChanged(this, [this](const Part* part) {
        onPartChanged(part);
    });

    m_notation->interaction()->selectionChanged().onNotify(this, [this]() {
        onSelectionChanged();
    });

    m_notation->interaction()->textEditingEnded().onReceive(this, [this](engraving::TextBase* text) {
        if (text && text->isHarmony()) {
            playElements({ text });
        }
    });

    m_notation->soloMuteState()->trackSoloMuteStateChanged().onReceive(
        this, [this](const InstrumentTrackId&, const notation::INotationSoloMuteState::SoloMuteState&) {
        updateSoloMuteStates();
    });
}

void PlaybackController::setMasterNotation(notation::IMasterNotationPtr masterNotation)
{
    if (m_masterNotation == masterNotation) {
        return;
    }

    if (m_masterNotation) {
        m_masterNotation->hasPartsChanged().disconnect(this);
        m_masterNotation->playback()->loopBoundariesChanged().disconnect(this);
        m_masterNotation->notation()->notationChanged().disconnect(this);
    }

    m_masterNotation = masterNotation;

    notifyActionCheckedChanged(LOOP_CODE);
    m_totalPlayTimeChanged.notify();

    if (!m_masterNotation) {
        return;
    }

    m_masterNotation->hasPartsChanged().onNotify(this, [this]() {
        m_isPlayAllowedChanged.notify();
    });

    m_masterNotation->playback()->loopBoundariesChanged().onNotify(this, [this]() {
        updateLoop();
    });

    // KitNote: the recording's properties (or measure 1, a drum key put before it) may have changed
    m_masterNotation->notation()->notationChanged().onNotify(this, [this]() {
        updateRecordingTrack();
    });

    updateRecordingTrack();
}

void PlaybackController::setIsExportingAudio(bool exporting)
{
    if (m_isExportingAudio == exporting) {
        return;
    }

    m_isExportingAudio = exporting;

    // KitNote: a score only has metronome events while its metronome is on. For the time of the export
    // the export option decides that, for this score alone: the metronome switch, which is a setting
    // shared with every other open window, is not touched.
    if (notationPlayback()) {
        notationPlayback()->setMetronomeOverride(exporting ? std::optional<bool>(exportAudioWithMetronome()) : std::nullopt);
    }

    updateSoloMuteStates();

    // KitNote: while audio is exported the click follows the export option; afterwards the metronome button again.
    if (notationPlayback() && notationConfiguration()
        && kfw::contains(m_instrumentTrackIdMap, notationPlayback()->metronomeTrackId())) {
        const bool click = exporting
                           ? exportAudioWithMetronome()
                           : notationConfiguration()->isMetronomeEnabled() || notationConfiguration()->isCountInEnabled();
        setTrackActivity(notationPlayback()->metronomeTrackId(), click);
    }

    if (exporting && notationPlayback()) {
        notationPlayback()->sendEventsForChangedTracks();
    }
}

bool PlaybackController::canReceiveAction(const ActionCode& code) const
{
    if (!m_masterNotation || !m_masterNotation->hasParts()) {
        return false;
    }

    static const std::unordered_set<ActionCode> REQUIRES_MEASURES {
        PLAY_CODE,
        PAUSE_CODE,
        PAUSE_AND_SELECT_CODE,
        STOP_CODE,
        REWIND_CODE,
        LOOP_CODE,
        LOOP_IN_CODE,
        LOOP_OUT_CODE,
    };

    if (kfw::contains(REQUIRES_MEASURES, code)) {
        const MasterScore* score = m_masterNotation->masterScore();
        return score && score->firstMeasure();
    }

    return true;
}

const std::map<TrackId, AudioResourceMeta>& PlaybackController::onlineSounds() const
{
    return m_onlineSoundsController->onlineSounds();
}

kfw::async::Notification PlaybackController::onlineSoundsChanged() const
{
    return m_onlineSoundsController->onlineSoundsChanged();
}

kfw::Progress PlaybackController::onlineSoundsProcessingProgress() const
{
    return m_onlineSoundsController->onlineSoundsProcessingProgress();
}

kfw::audio::secs_t PlaybackController::playedTickToSecs(int tick) const
{
    return secs_t(notationPlayback()->playedTickToSec(tick));
}

// --- KitNote: the recording of the score ---------------------------------------------------------------

const IPlaybackController::RecordingState& PlaybackController::recordingState() const
{
    return m_recordingState;
}

kfw::async::Notification PlaybackController::recordingStateChanged() const
{
    return m_recordingStateChanged;
}

//! KitNote: a recording that was not there (moved, on a drive not plugged in, being copied) or could not be
//! read is looked for again - when playback starts, and when the Mixer or the recording's dialog is opened,
//! or KitNote comes to the front again - so that one put back is played without the score being reopened.
//! A file that is still missing costs one look; one that failed to load is tried again only once it has
//! changed (another size or time).
void PlaybackController::recheckRecording()
{
    if (!m_recordingState.attached || !m_recordingState.failed || m_currentSequenceId == -1 || !m_masterNotation) {
        return;
    }

    const ScoreRecording recording = scoreRecording(m_masterNotation->masterScore());
    const project::INotationProjectPtr project = globalContext()->currentProject();
    const QString scorePath = project && !project->isNewlyCreated() ? project->path().toQString() : QString();
    const QString found = recording.isSet() ? resolveRecordingPath(recording.path, scorePath) : QString();
    const QFileInfo info(found);
    if (found.isEmpty() || !info.isFile() || !info.isReadable()) {
        return;
    }

    const QString stamp = found + u'|' + QString::number(info.size()) + u'|'
                          + QString::number(info.lastModified().toMSecsSinceEpoch());
    if (found == m_recordingFile && stamp == m_recordingRechecked) {
        return;     // the file that could not be read, unchanged
    }
    m_recordingRechecked = stamp;

    LOGI() << "KitNote: the recording is there again: " << found;
    m_recordingStoredPath.clear();      // looked for anew,
    m_recordingFile.clear();            // and added again
    updateRecordingTrack();
}

void PlaybackController::setRecordingState(const RecordingState& state)
{
    const RecordingState& was = m_recordingState;
    if (was.attached == state.attached && was.filePath == state.filePath && was.loaded == state.loaded && was.failed == state.failed
        && was.mutedBySpeed == state.mutedBySpeed && kfw::RealIsEqual(was.durationSecs, state.durationSecs)
        && kfw::RealIsEqual(was.offsetSecs, state.offsetSecs)) {
        return;
    }
    m_recordingState = state;
    m_recordingStateChanged.notify();
}

//! The recording the score names, in the sequence: added, replaced, taken away, or given its new offset.
void PlaybackController::updateRecordingTrack()
{
    if (m_currentSequenceId == -1 || !m_masterNotation || !audioSettings()) {
        return;
    }

    const ScoreRecording recording = scoreRecording(m_masterNotation->masterScore());
    const project::INotationProjectPtr project = globalContext()->currentProject();
    const QString scorePath = project && !project->isNewlyCreated() ? project->path().toQString() : QString();

    // (this runs after every edit: the file is looked for only when the score names another one, or moved)
    if (recording.path != m_recordingStoredPath || scorePath != m_recordingScorePath) {
        m_recordingStoredPath = recording.path;
        m_recordingScorePath = scorePath;
        m_recordingFound = recording.isSet() ? resolveRecordingPath(recording.path, scorePath) : QString();
    }
    const QString file = m_recordingFound;

    if (file != m_recordingFile || recording.isSet() != m_recordingState.attached) {
        removeRecordingTrack();
        m_recordingFile = file;

        RecordingState state;
        state.attached = recording.isSet();
        state.filePath = file;
        state.failed = recording.isSet() && file.isEmpty();
        state.offsetSecs = recording.offset;
        state.mutedBySpeed = recording.isSet() && !kfw::RealIsEqual(tempoMultiplier(), 1.0);
        setRecordingState(state);

        // (one that is not found has its channel too, silent: the Mixer says what is wrong, and a click there
        // chooses it again)
        if (recording.isSet()) {
            addRecordingTrack(file);
        }
        return;
    }

    if (!kfw::RealIsEqual(m_recordingState.offsetSecs, recording.offset)) {
        RecordingState state = m_recordingState;
        state.offsetSecs = recording.offset;
        setRecordingState(state);
    }

    // the same file: the offset may have moved, or measure 1 (a drum key put before it)
    auto it = m_instrumentTrackIdMap.find(recordingTrackId());
    if (it != m_instrumentTrackIdMap.end()) {
        AudioInputParams params = recordingInputParams();
        const msecs_t offset = std::stoll(params.configuration[SOUND_TRACK_OFFSET_KEY]);
        if (offset != m_recordingEngineOffset) {
            m_recordingEngineOffset = offset;
            playback()->setInputParams(m_currentSequenceId, it->second, params);
        }
    }
}

//! recording time = playback time + (where measure 1 is in the recording - where it is in the playback)
AudioInputParams PlaybackController::recordingInputParams() const
{
    const engraving::MasterScore* score = m_masterNotation ? m_masterNotation->masterScore() : nullptr;
    const double offset = scoreRecording(score).offset - measureOneSecs(score);

    AudioInputParams params;
    params.configuration[SOUND_TRACK_OFFSET_KEY] = std::to_string(std::llround(offset * 1000000.0));
    return params;
}

void PlaybackController::addRecordingTrack(const QString& filePath)
{
    // the engine reads the file's bytes (in a thread of its own) and frees them; of a file that cannot be
    // read it gets none, and says so
    QFile file(filePath);
    const QByteArray bytes = !filePath.isEmpty() && file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
    if (bytes.isEmpty()) {
        RecordingState state = m_recordingState;
        state.failed = true;
        setRecordingState(state);
    }
    kfw::io::Buffer* device = new kfw::io::Buffer(kfw::ByteArray::fromQByteArray(bytes));

    AudioInputParams inParams = recordingInputParams();
    m_recordingEngineOffset = std::stoll(inParams.configuration[SOUND_TRACK_OFFSET_KEY]);
    AudioOutputParams outParams = audioSettings()->trackOutputParams(recordingTrackId());

    // silent from its first block when it is not to be heard: on the command line the score is only
    // played to be exported, and the export may be on its way before the track is there to be muted
    const bool exporting = m_isExportingAudio
                           || (application() && application()->runMode() == kfw::IApplication::RunMode::ConsoleApp);
    const bool audible = kfw::RealIsEqual(tempoMultiplier(), 1.0) && !(exporting && !exportAudioWithRecording());
    outParams.forceMute = !audible;
    outParams.muted = !audible || (m_notation && m_notation->soloMuteState()->trackSoloMuteState(recordingTrackId()).mute);

    const uint64_t request = ++m_recordingRequest;
    const uint64_t playbackKey = notationPlaybackKey();
    const TrackSequenceId sequenceId = m_currentSequenceId;

    playback()->addTrack(sequenceId, kfw::trc("playback", "Recording"), device, { std::move(inParams), std::move(outParams) })
    .onResolve(this, [this, request, playbackKey, sequenceId](const TrackId trackId, const AudioParams& appliedParams) {
        if (notationPlaybackKey() != playbackKey || sequenceId != m_currentSequenceId) {
            return;     // the sequence is gone with its tracks
        }
        if (request != m_recordingRequest) {
            playback()->removeTrack(sequenceId, trackId);       // another recording was asked for meanwhile
            return;
        }

        m_instrumentTrackIdMap.insert({ recordingTrackId(), trackId });

        // (input params of its own it has none; without them its volume would not be saved with the score)
        audioSettings()->setTrackInputParams(recordingTrackId(), AudioInputParams());
        audioSettings()->setTrackOutputParams(recordingTrackId(), appliedParams.out);

        updateSoloMuteStates();
        m_trackAdded.send(trackId);
    })
    .onReject(this, [this, request](int code, const std::string& msg) {
        LOGE() << "KitNote: the recording could not be added, code: [" << code << "] " << msg;
        if (request == m_recordingRequest) {
            RecordingState state = m_recordingState;
            state.failed = true;
            setRecordingState(state);
        }
    });
}

void PlaybackController::removeRecordingTrack()
{
    ++m_recordingRequest;

    auto it = m_instrumentTrackIdMap.find(recordingTrackId());
    if (it == m_instrumentTrackIdMap.end()) {
        return;
    }

    const TrackId trackId = it->second;
    m_instrumentTrackIdMap.erase(it);
    playback()->removeTrack(m_currentSequenceId, trackId);
    m_trackRemoved.send(trackId);
    updateSoloMuteStates();
}

void PlaybackController::onRecordingInputParamsChanged(const AudioInputParams& params)
{
    const auto value = [&params](const char* key) -> std::string {
        auto it = params.configuration.find(key);
        return it != params.configuration.end() ? it->second : std::string();
    };

    // (before the file is decoded the engine does not say: what is known stays - a file not found has failed)
    RecordingState state = m_recordingState;
    const std::string decoded = value(SOUND_TRACK_STATE_KEY);
    if (!decoded.empty()) {
        state.loaded = decoded == "ready";
        state.failed = decoded == "failed";
    }
    const std::string duration = value(SOUND_TRACK_DURATION_KEY);
    state.durationSecs = duration.empty() ? 0.0 : std::stoll(duration) / 1000000.0;
    setRecordingState(state);
}
