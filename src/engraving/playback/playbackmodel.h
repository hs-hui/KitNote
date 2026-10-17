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

#include <unordered_map>
#include <map>
#include <functional>

#include "async/asyncable.h"
#include "async/channel.h"
#include "async/notification.h"
#include "types/id.h"
#include "modularity/ioc.h"
#include "mpe/events.h"
#include "mpe/iarticulationprofilesrepository.h"

#include "../types/types.h"
#include "playbackeventsrenderer.h"
#include "playbacksetupdataresolver.h"
#include "playbackcontext.h"

namespace kn::engraving {
class Score;
class Note;
class EngravingItem;
class Segment;
class Instrument;
class RepeatList;

class PlaybackModel : public kfw::Contextable, public kfw::async::Asyncable
{
public:
    kfw::ContextInject<kfw::mpe::IArticulationProfilesRepository> profilesRepository = { this };

public:
    PlaybackModel(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void load(Score* score);
    void reload();

    void setSendEventsOnScoreChange(const InstrumentTrackId& trackId, bool send);
    void sendEventsForChangedTracks();

    kfw::async::Channel<InstrumentTrackIdSet> tracksDataChanged() const;

    bool isPlayRepeatsEnabled() const;
    void setPlayRepeats(const bool isEnabled);

    bool isPlayChordSymbolsEnabled() const;
    void setPlayChordSymbols(const bool isEnabled);

    bool useScoreDynamicsForOffstreamPlayback() const;
    void setUseScoreDynamicsForOffstreamPlayback(bool use);

    bool isMetronomeEnabled() const;
    void setIsMetronomeEnabled(const bool isEnabled);

    const InstrumentTrackId& metronomeTrackId() const;
    InstrumentTrackId chordSymbolsTrackId(const ID& partId) const;
    bool isChordSymbolsTrack(const InstrumentTrackId& trackId) const;

    bool hasSoundFlags(const InstrumentTrackId& trackId) const;

    kfw::mpe::PlaybackData& resolveTrackPlaybackData(const InstrumentTrackId& trackId);
    kfw::mpe::PlaybackData& resolveTrackPlaybackData(const ID& partId, const String& instrumentId);

    void triggerEventsForItems(const std::vector<const EngravingItem*>& items, kfw::mpe::duration_t duration, bool flushSound);
    void triggerMetronome(int tick);
    void triggerCountIn(int tick, kfw::mpe::duration_t& countInDuration);

    InstrumentTrackIdSet existingTrackIdSet() const;
    kfw::async::Channel<InstrumentTrackId> trackAdded() const;
    kfw::async::Channel<InstrumentTrackId> trackRemoved() const;

private:
    static const InstrumentTrackId METRONOME_TRACK_ID;
    static const InstrumentTrackId CHORD_SYMBOLS_TRACK_ID;

    using ChangedTrackIdSet = InstrumentTrackIdSet;

    struct TickBoundaries
    {
        int tickFrom = -1;
        int tickTo = -1;
    };

    struct TrackBoundaries
    {
        track_idx_t trackFrom = kfw::nidx;
        track_idx_t trackTo = kfw::nidx;
    };

    InstrumentTrackId idKey(const EngravingItem* item) const;
    InstrumentTrackId idKey(const std::vector<const EngravingItem*>& items) const;
    InstrumentTrackId idKey(const ID& partId, const String& instrumentId) const;

    void update(const int tickFrom, const int tickTo, const track_idx_t trackFrom, const track_idx_t trackTo,
                ChangedTrackIdSet* trackChanges = nullptr);
    void updateSetupData();
    void updateContext(const track_idx_t trackFrom, const track_idx_t trackTo);
    void updateContext(const InstrumentTrackId& trackId);
    void updateEvents(const int tickFrom, const int tickTo, const track_idx_t trackFrom, const track_idx_t trackTo,
                      ChangedTrackIdSet* trackChanges = nullptr);

    void reloadMetronomeEvents();

    void processSegment(const int tickPositionOffset, const Segment* segment, const std::set<staff_idx_t>& staffIdxSet,
                        bool isFirstChordRestSegmentOfMeasure, ChangedTrackIdSet* trackChanges);
    void processMeasureRepeat(const int tickPositionOffset, const MeasureRepeat* measureRepeat, const Measure* currentMeasure,
                              const staff_idx_t staffIdx, ChangedTrackIdSet* trackChanges);

    bool hasToReloadTracks(const ScoreChanges& changes) const;
    bool hasToReloadScore(const ScoreChanges& changes) const;

    void clearExpiredTracks();
    void clearExpiredContexts(const track_idx_t trackFrom, const track_idx_t trackTo);
    void clearExpiredEvents(const int tickFrom, const int tickTo, const track_idx_t trackFrom, const track_idx_t trackTo,
                            ChangedTrackIdSet* trackChanges = nullptr);
    void collectChangesTracks(const InstrumentTrackId& trackId, ChangedTrackIdSet* result);
    void notifyAboutChanges(const InstrumentTrackIdSet& oldTracks, const InstrumentTrackIdSet& changedTracks);

    void sendEvents(const InstrumentTrackId& trackId);

    void removeEventsFromRange(const track_idx_t trackFrom, const track_idx_t trackTo, const kfw::mpe::timestamp_t timestampFrom = -1,
                               const kfw::mpe::timestamp_t timestampTo = -1, ChangedTrackIdSet* trackChanges = nullptr);
    void removeTrackEvents(const InstrumentTrackId& trackId, const kfw::mpe::timestamp_t timestampFrom = -1,
                           const kfw::mpe::timestamp_t timestampTo = -1, ChangedTrackIdSet* trackChanges = nullptr);

    bool shouldSkipChanges(const ScoreChanges& changes) const;

    TrackBoundaries trackBoundaries(const ScoreChanges& changes) const;
    TickBoundaries tickBoundaries(const ScoreChanges& changes) const;

    const RepeatList& repeatList() const;

    kfw::mpe::ArticulationsProfilePtr defaultActiculationProfile(const InstrumentTrackId& trackId) const;

    PlaybackContextPtr playbackCtx(const InstrumentTrackId& trackId);

    static void applyTiedNotesTickBoundaries(const Note* note, TickBoundaries& tickBoundaries);
    static void applyTieTickBoundaries(const Tie* tie, TickBoundaries& tickBoundaries);

    Score* m_score = nullptr;
    bool m_expandRepeats = true;
    bool m_playChordSymbols = true;
    bool m_useScoreDynamicsForOffstreamPlayback = true;
    bool m_metronomeEnabled = true;

    PlaybackEventsRenderer m_renderer;
    PlaybackSetupDataResolver m_setupResolver;

    std::unordered_map<InstrumentTrackId, PlaybackContextPtr> m_playbackCtxMap;
    std::unordered_map<InstrumentTrackId, kfw::mpe::PlaybackData> m_playbackDataMap;
    std::unordered_map<InstrumentTrackId, bool> m_sendEventsOnScoreChangeMap;

    InstrumentTrackIdSet m_changedTrackIdSet;

    kfw::async::Channel<InstrumentTrackIdSet> m_tracksDataChanged;
    kfw::async::Channel<InstrumentTrackId> m_trackAdded;
    kfw::async::Channel<InstrumentTrackId> m_trackRemoved;
};
}
