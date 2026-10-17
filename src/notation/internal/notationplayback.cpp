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
#include "notationplayback.h"

#include <cmath>

#include "engraving/dom/chord.h"
#include "engraving/dom/chordrest.h"
#include "engraving/dom/factory.h"
#include "engraving/dom/instrument.h"
#include "engraving/dom/linkedobjects.h"
#include "engraving/dom/masterscore.h"
#include "engraving/dom/measure.h"
#include "engraving/dom/note.h"
#include "engraving/dom/page.h"
#include "engraving/dom/part.h"
#include "engraving/dom/repeatlist.h"
#include "engraving/dom/segment.h"
#include "engraving/dom/sig.h"
#include "engraving/dom/soundflag.h"
#include "engraving/dom/staff.h"
#include "engraving/dom/stafftext.h"
#include "engraving/dom/tempo.h"
#include "engraving/dom/tempotext.h"
#include "engraving/dom/utils.h"

#include "notationerrors.h"

#include "log.h"

using namespace kn;
using namespace kn::notation;
using namespace kn::engraving;
using namespace kfw;
using namespace kfw::midi;
using namespace kfw::async;

static constexpr double PLAYBACK_TAIL_SECS = 3;

NotationPlayback::NotationPlayback(IGetScore* getScore,
                                   kfw::async::Notification notationChanged,
                                   const modularity::ContextPtr& iocCtx)
    : kfw::Contextable(iocCtx), m_getScore(getScore), m_notationChanged(notationChanged), m_playbackModel(iocCtx)
{
    m_notationChanged.onNotify(this, [this]() {
        updateLoopBoundaries();
    });
}

kn::engraving::Score* NotationPlayback::score() const
{
    return m_getScore->score();
}

void NotationPlayback::init()
{
    IF_ASSERT_FAILED(score()) {
        return;
    }

    m_playbackModel.setPlayRepeats(configuration()->isPlayRepeatsEnabled());
    m_playbackModel.setPlayChordSymbols(configuration()->isPlayChordSymbolsEnabled());
    m_playbackModel.setUseScoreDynamicsForOffstreamPlayback(configuration()->playPreviewNotesWithScoreDynamics());
    m_playbackModel.setIsMetronomeEnabled(configuration()->isMetronomeEnabled());

    m_playbackModel.load(score());

    updateTotalPlayTime();
    m_playbackModel.tracksDataChanged().onReceive(this, [this](const InstrumentTrackIdSet&) {
        updateTotalPlayTime();
    });

    configuration()->isPlayRepeatsChanged().onNotify(this, [this]() {
        bool expandRepeats = configuration()->isPlayRepeatsEnabled();
        if (expandRepeats != m_playbackModel.isPlayRepeatsEnabled()) {
            m_playbackModel.setPlayRepeats(expandRepeats);
            m_playbackModel.reload();
        }
    });

    configuration()->isPlayChordSymbolsChanged().onNotify(this, [this]() {
        bool playChordSymbols = configuration()->isPlayChordSymbolsEnabled();
        if (playChordSymbols != m_playbackModel.isPlayChordSymbolsEnabled()) {
            m_playbackModel.setPlayChordSymbols(playChordSymbols);
            m_playbackModel.reload();
        }
    });

    configuration()->playPreviewNotesWithScoreDynamicsChanged().onNotify(this, [this]() {
        bool useScoreDynamics = configuration()->playPreviewNotesWithScoreDynamics();
        if (useScoreDynamics != m_playbackModel.useScoreDynamicsForOffstreamPlayback()) {
            m_playbackModel.setUseScoreDynamicsForOffstreamPlayback(useScoreDynamics);
        }
    });

    configuration()->isMetronomeEnabledChanged().onNotify(this, [this]() {
        if (m_metronomeOverride.has_value()) {
            return; // KitNote: an audio export decides for now; the switch is read again when it ends
        }

        bool metronomeEnabled = configuration()->isMetronomeEnabled();
        if (metronomeEnabled != m_playbackModel.isMetronomeEnabled()) {
            m_playbackModel.setIsMetronomeEnabled(metronomeEnabled);
        }
    });

    score()->loopBoundaryTickChanged().onReceive(this, [this](LoopBoundaryType, unsigned) {
        updateLoopBoundaries();
    });
}

void NotationPlayback::reload()
{
    m_playbackModel.reload();
}

void NotationPlayback::setSendEventsOnScoreChange(const InstrumentTrackId& trackId, bool send)
{
    m_playbackModel.setSendEventsOnScoreChange(trackId, send);
}

void NotationPlayback::sendEventsForChangedTracks()
{
    m_playbackModel.sendEventsForChangedTracks();
}

kfw::async::Channel<InstrumentTrackIdSet> NotationPlayback::tracksDataChanged() const
{
    return m_playbackModel.tracksDataChanged();
}

const engraving::InstrumentTrackId& NotationPlayback::metronomeTrackId() const
{
    return m_playbackModel.metronomeTrackId();
}

engraving::InstrumentTrackId NotationPlayback::chordSymbolsTrackId(const ID& partId) const
{
    return m_playbackModel.chordSymbolsTrackId(partId);
}

bool NotationPlayback::isChordSymbolsTrack(const engraving::InstrumentTrackId& trackId) const
{
    return m_playbackModel.isChordSymbolsTrack(trackId);
}

const kfw::mpe::PlaybackData& NotationPlayback::trackPlaybackData(const engraving::InstrumentTrackId& trackId) const
{
    return m_playbackModel.resolveTrackPlaybackData(trackId);
}

void NotationPlayback::triggerEventsForItems(const std::vector<const EngravingItem*>& items, kfw::mpe::duration_t duration,
                                             bool flushSound)
{
    m_playbackModel.triggerEventsForItems(items, duration, flushSound);
}

void NotationPlayback::triggerMetronome(kfw::midi::tick_t tick)
{
    m_playbackModel.triggerMetronome(tick);
}

void NotationPlayback::setMetronomeOverride(std::optional<bool> enabled)
{
    m_metronomeOverride = enabled;
    m_playbackModel.setIsMetronomeEnabled(enabled.has_value() ? enabled.value() : configuration()->isMetronomeEnabled());
}

void NotationPlayback::triggerCountIn(kfw::midi::tick_t tick, kfw::secs_t& countInDuration)
{
    kfw::mpe::duration_t durationInMicrosecs = 0;
    m_playbackModel.triggerCountIn(tick, durationInMicrosecs);
    countInDuration = audio::microsecsToSecs(durationInMicrosecs);
}

void NotationPlayback::triggerControllers(const kfw::mpe::ControllerChangeEventList& list, notation::staff_idx_t staffIdx, int tick)
{
    if (list.empty()) {
        return;
    }

    const Staff* staff = score()->staff(staffIdx);
    if (!staff) {
        return;
    }

    const Part* part = staff->part();
    const InstrumentTrackId trackId {
        part->id(),
        part->instrumentId(Fraction::fromTicks(tick))
    };

    const mpe::PlaybackEventsMap events {
        { 0, mpe::PlaybackEventList(list.begin(), list.end()) }
    };

    mpe::PlaybackData& data = m_playbackModel.resolveTrackPlaybackData(trackId);
    data.offStream.send(events, {}, false /*flushOffstream*/);
}

InstrumentTrackIdSet NotationPlayback::existingTrackIdSet() const
{
    return m_playbackModel.existingTrackIdSet();
}

kfw::async::Channel<InstrumentTrackId> NotationPlayback::trackAdded() const
{
    return m_playbackModel.trackAdded();
}

kfw::async::Channel<InstrumentTrackId> NotationPlayback::trackRemoved() const
{
    return m_playbackModel.trackRemoved();
}

void NotationPlayback::updateLoopBoundaries()
{
    LoopBoundaries newBoundaries;
    newBoundaries.loopInTick = score()->loopInTick();
    newBoundaries.loopOutTick = score()->loopOutTick();
    newBoundaries.enabled = m_loopBoundaries.enabled;

    if (m_loopBoundaries != newBoundaries) {
        m_loopBoundaries = newBoundaries;
        m_loopBoundariesChanged.notify();
    }
}

void NotationPlayback::updateTotalPlayTime()
{
    const kn::engraving::Score* score = m_getScore->score();
    if (!score) {
        return;
    }

    int lastTick = score->repeatList(m_playbackModel.isPlayRepeatsEnabled()).ticks();
    audio::secs_t newPlayTime = score->utick2utime(lastTick);
    newPlayTime += PLAYBACK_TAIL_SECS;

    if (m_totalPlayTime == newPlayTime) {
        return;
    }

    m_totalPlayTime = newPlayTime;
    m_totalPlayTimeChanged.send(m_totalPlayTime);
}

kfw::audio::secs_t NotationPlayback::totalPlayTime() const
{
    return m_totalPlayTime;
}

kfw::async::Channel<kfw::audio::secs_t> NotationPlayback::totalPlayTimeChanged() const
{
    return m_totalPlayTimeChanged;
}

kfw::audio::secs_t NotationPlayback::playedTickToSec(tick_t tick) const
{
    return score() ? score()->utick2utime(tick) : 0.0;
}

tick_t NotationPlayback::secToPlayedTick(kfw::audio::secs_t sec) const
{
    if (!score()) {
        return 0;
    }

    return score()->utime2utick(sec);
}

tick_t NotationPlayback::secToTick(kfw::audio::secs_t sec) const
{
    if (!score()) {
        return 0;
    }

    tick_t utick = secToPlayedTick(sec);

    return score()->repeatList(m_playbackModel.isPlayRepeatsEnabled()).utick2tick(utick);
}

RetVal<kfw::midi::tick_t> NotationPlayback::playPositionTickByRawTick(kfw::midi::tick_t tick) const
{
    if (!score()) {
        return make_ret(Err::Undefined);
    }

    kfw::midi::tick_t playbackTick = score()->repeatList(m_playbackModel.isPlayRepeatsEnabled()).tick2utick(tick);

    return RetVal<kfw::midi::tick_t>::make_ok(std::move(playbackTick));
}

RetVal<kfw::midi::tick_t> NotationPlayback::playPositionTickByElement(const EngravingItem* element) const
{
    IF_ASSERT_FAILED(element) {
        return make_ret(Err::Undefined);
    }

    if (!score()) {
        return make_ret(Err::Undefined);
    }

    return playPositionTickByRawTick(element->tick().ticks());
}

void NotationPlayback::addLoopBoundary(LoopBoundaryType boundaryType, tick_t tick)
{
    const Measure* first = score()->firstMeasure();
    const Measure* last = score()->lastMeasure();
    IF_ASSERT_FAILED(first && last) {
        return;
    }

    if (tick == BoundaryTick::FirstScoreTick) {
        tick = first->tick().ticks();
    } else if (tick == BoundaryTick::LastScoreTick) {
        tick = last->endTick().ticks();
    }

    switch (boundaryType) {
    case LoopBoundaryType::LoopIn:
        addLoopIn(tick);
        break;
    case LoopBoundaryType::LoopOut:
        addLoopOut(tick);
        break;
    case LoopBoundaryType::Unknown:
        break;
    }
}

void NotationPlayback::addLoopIn(int _tick)
{
    Fraction tick = Fraction::fromTicks(_tick);

    if (_tick == BoundaryTick::SelectedNoteTick) {
        tick = score()->pos();
    }

    if (tick >= score()->loopOutTick()) { // If In pos >= Out pos, reset Out pos to end of score
        score()->setLoopOutTick(score()->lastMeasure()->endTick());
    }

    score()->setLoopInTick(tick);
}

void NotationPlayback::addLoopOut(int _tick)
{
    Fraction tick = Fraction::fromTicks(_tick);

    if (_tick == BoundaryTick::SelectedNoteTick) {
        tick = score()->pos() + score()->inputState().ticks();
    }

    if (tick <= score()->loopInTick()) { // If Out pos <= In pos, reset In pos to beginning of score
        score()->setLoopInTick(Fraction(0, 1));
    } else {
        if (tick > score()->lastMeasure()->endTick()) {
            tick = score()->lastMeasure()->endTick();
        }
    }

    score()->setLoopOutTick(tick);
}

void NotationPlayback::setLoopBoundariesEnabled(bool enabled)
{
    if (m_loopBoundaries.enabled == enabled) {
        return;
    }

    m_loopBoundaries.enabled = enabled;
    m_loopBoundariesChanged.notify();
}

bool NotationPlayback::isLoopEnabled() const
{
    return !m_loopBoundaries.isNull() && m_loopBoundaries.enabled;
}

const LoopBoundaries& NotationPlayback::loopBoundaries() const
{
    return m_loopBoundaries;
}

Notification NotationPlayback::loopBoundariesChanged() const
{
    return m_loopBoundariesChanged;
}

const Tempo& NotationPlayback::multipliedTempo(tick_t tick) const
{
    if (!score()) {
        static Tempo empty;
        return empty;
    }

    // KitNote: the measures of a drum key (see NotationInteraction::insertDrumKey) run at a hidden speed of
    // their own, so that playback does not wait for them. What is shown there is the tempo the music starts with.
    for (const kn::engraving::Measure* measure = score()->firstMeasure(); measure && measure->irregular();
         measure = measure->nextMeasure()) {
        bool keyTempo = false;
        bool sounding = false;
        for (const kn::engraving::Segment* segment = measure->first(kn::engraving::SegmentType::ChordRest); segment;
             segment = segment->next(kn::engraving::SegmentType::ChordRest)) {
            for (const kn::engraving::EngravingItem* annotation : segment->annotations()) {
                keyTempo |= annotation->isTempoText() && !annotation->visible()
                            && kn::engraving::toTempoText(annotation)->isTempoPrimo();
            }
            for (const kn::engraving::EngravingItem* item : segment->elist()) {
                if (item && item->isChord()) {
                    for (const kn::engraving::Note* note : kn::engraving::toChord(item)->notes()) {
                        sounding |= note->play();
                    }
                }
            }
        }
        if (!keyTempo || sounding) {
            break;
        }
        tick = std::max(tick, static_cast<tick_t>(measure->endTick().ticks()));
    }

    m_currentTempo.valueBpm = static_cast<int>(std::round(score()->tempomap()->multipliedTempo(tick).toBPM().val));

    // KitNote: in 6/8, 9/8 and 12/8 the beat is a dotted quarter, as the tempo mark there says ("♩. = 60")
    const kn::engraving::Fraction signature = score()->sigmap()->timesig(static_cast<int>(tick)).nominal();
    const bool compound = signature.denominator() == 8 && signature.numerator() % 3 == 0 && signature.numerator() > 3;
    m_currentTempo.withDot = compound;
    if (compound) {
        m_currentTempo.valueBpm = static_cast<int>(std::round(score()->tempomap()->multipliedTempo(tick).toBPM().val / 1.5));
    }

    return m_currentTempo;
}

MeasureBeat NotationPlayback::beat(tick_t tick) const
{
    return kn::engraving::findBeat(m_getScore->score(), tick);
}

tick_t NotationPlayback::beatToRawTick(int measureIndex, int beatIndex) const
{
    return score() ? score()->sigmap()->bar2tick(measureIndex, beatIndex) : 0;
}

double NotationPlayback::tempoMultiplier() const
{
    return score() ? score()->tempomap()->tempoMultiplier().val : 1.0;
}

void NotationPlayback::setTempoMultiplier(double multiplier)
{
    Score* score = this->score();
    if (!score) {
        return;
    }

    if (!score->tempomap()->setTempoMultiplier(multiplier)) {
        return;
    }

    score->masterScore()->updateRepeatListTempo();

    m_playbackModel.reload();
}

void NotationPlayback::addSoundFlags(const std::vector<StaffText*>& staffTextList)
{
    TRACEFUNC;

    if (staffTextList.empty()) {
        return;
    }

    bool added = false;

    for (StaffText* staffText : staffTextList) {
        added |= doAddSoundFlag(staffText);
    }

    if (added) {
        score()->update();
        m_notationChanged.notify();
    }
}

bool NotationPlayback::doAddSoundFlag(StaffText* staffText)
{
    IF_ASSERT_FAILED(staffText) {
        return false;
    }

    if (staffText->hasSoundFlag()) {
        return false;
    }

    SoundFlag* soundFlag = Factory::createSoundFlag(staffText);
    staffText->add(soundFlag);

    const LinkedObjects* links = staffText->links();
    if (!links) {
        return true;
    }

    for (EngravingObject* obj : *links) {
        if (obj && obj->isStaffText() && obj != staffText) {
            toStaffText(obj)->add(soundFlag->linkedClone());
        }
    }

    return true;
}

void NotationPlayback::removeSoundFlags(const InstrumentTrackIdSet& trackIdSet)
{
    TRACEFUNC;

    std::vector<StaffText*> staffTextList = collectStaffText(trackIdSet, true /*withSoundFlags*/);
    if (staffTextList.empty()) {
        return;
    }

    for (StaffText* staffText : staffTextList) {
        if (!staffText->hasSoundFlag()) {
            continue;
        }

        staffText->remove(staffText->soundFlag());

        const LinkedObjects* links = staffText->links();
        if (!links) {
            continue;
        }

        for (EngravingObject* obj : *links) {
            if (obj && obj->isStaffText() && obj != staffText) {
                StaffText* linkedStaffText = toStaffText(obj);
                if (!linkedStaffText->hasSoundFlag()) {
                    continue;
                }

                linkedStaffText->remove(linkedStaffText->soundFlag());
            }
        }
    }

    score()->update();

    m_playbackModel.reload();
    m_notationChanged.notify();
}

bool NotationPlayback::hasSoundFlags(const engraving::InstrumentTrackIdSet& trackIdSet)
{
    TRACEFUNC;

    for (const InstrumentTrackId& trackId : trackIdSet) {
        if (m_playbackModel.hasSoundFlags(trackId)) {
            return true;
        }
    }

    return false;
}

std::vector<StaffText*> NotationPlayback::collectStaffText(const InstrumentTrackIdSet& trackIdSet, bool withSoundFlags) const
{
    TRACEFUNC;

    std::vector<StaffText*> result;

    if (trackIdSet.empty()) {
        return result;
    }

    const Score* score = this->score();
    IF_ASSERT_FAILED(score) {
        return result;
    }

    const Measure* fm = score->firstMeasure();
    if (!fm) {
        return result;
    }

    for (const Segment* seg = fm->first(SegmentType::ChordRest); seg; seg = seg->next1(SegmentType::ChordRest)) {
        for (EngravingItem* annotation : seg->annotations()) {
            if (!annotation || !annotation->isStaffText()) {
                continue;
            }

            StaffText* staffText = toStaffText(annotation);
            bool hasSoundFlag = staffText->hasSoundFlag();

            if (withSoundFlags && !hasSoundFlag) {
                continue;
            }

            if (!withSoundFlags && hasSoundFlag) {
                continue;
            }

            InstrumentTrackId trackId = kn::engraving::makeInstrumentTrackId(annotation);
            if (kfw::contains(trackIdSet, trackId)) {
                result.push_back(staffText);
            }
        }
    }

    return result;
}
