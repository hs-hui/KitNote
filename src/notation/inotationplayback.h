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
#ifndef KN_NOTATION_INOTATIONPLAYBACK_H
#define KN_NOTATION_INOTATIONPLAYBACK_H

#include <optional>

#include "types/retval.h"
#include "midi/miditypes.h"
#include "audio/common/audiotypes.h"
#include "async/channel.h"
#include "mpe/events.h"
#include "engraving/types/types.h"

#include "notationtypes.h"

namespace kn::notation {
class INotationPlayback
{
public:
    virtual ~INotationPlayback() = default;

    virtual void init() = 0;
    virtual void reload() = 0;

    virtual void setSendEventsOnScoreChange(const InstrumentTrackId& trackId, bool send) = 0;
    virtual void sendEventsForChangedTracks() = 0;

    virtual kfw::async::Channel<InstrumentTrackIdSet> tracksDataChanged() const = 0;

    virtual const engraving::InstrumentTrackId& metronomeTrackId() const = 0;
    virtual engraving::InstrumentTrackId chordSymbolsTrackId(const kfw::ID& partId) const = 0;
    virtual bool isChordSymbolsTrack(const engraving::InstrumentTrackId& trackId) const = 0;

    virtual const kfw::mpe::PlaybackData& trackPlaybackData(const engraving::InstrumentTrackId& trackId) const = 0;

    virtual void triggerEventsForItems(const std::vector<const EngravingItem*>& items, kfw::mpe::duration_t duration, bool flushSound) = 0;
    virtual void triggerMetronome(kfw::midi::tick_t tick) = 0;

    //! KitNote: while a value is set, the score has metronome events (or has none) whatever the metronome
    //! switch says. Used for the time of an audio export; std::nullopt hands control back to the switch.
    virtual void setMetronomeOverride(std::optional<bool> enabled) = 0;
    virtual void triggerCountIn(kfw::midi::tick_t tick, kfw::secs_t& countInDuration) = 0;
    virtual void triggerControllers(const kfw::mpe::ControllerChangeEventList& list, notation::staff_idx_t staffIdx, int tick) = 0;

    virtual engraving::InstrumentTrackIdSet existingTrackIdSet() const = 0;
    virtual kfw::async::Channel<engraving::InstrumentTrackId> trackAdded() const = 0;
    virtual kfw::async::Channel<engraving::InstrumentTrackId> trackRemoved() const = 0;

    virtual kfw::audio::secs_t totalPlayTime() const = 0;
    virtual kfw::async::Channel<kfw::audio::secs_t> totalPlayTimeChanged() const = 0;

    virtual kfw::audio::secs_t playedTickToSec(kfw::midi::tick_t tick) const = 0;
    virtual kfw::midi::tick_t secToPlayedTick(kfw::audio::secs_t sec) const = 0;
    virtual kfw::midi::tick_t secToTick(kfw::audio::secs_t sec) const = 0;

    virtual kfw::RetVal<kfw::midi::tick_t> playPositionTickByRawTick(kfw::midi::tick_t tick) const = 0;
    virtual kfw::RetVal<kfw::midi::tick_t> playPositionTickByElement(const EngravingItem* element) const = 0;

    enum BoundaryTick {
        FirstScoreTick = 0,
        SelectedNoteTick,
        LastScoreTick
    };

    virtual void addLoopBoundary(LoopBoundaryType boundaryType, kfw::midi::tick_t tick) = 0;
    virtual void setLoopBoundariesEnabled(bool enabled) = 0;
    virtual bool isLoopEnabled() const = 0;
    virtual const LoopBoundaries& loopBoundaries() const = 0;
    virtual kfw::async::Notification loopBoundariesChanged() const = 0;

    virtual const Tempo& multipliedTempo(kfw::midi::tick_t tick) const = 0;
    virtual MeasureBeat beat(kfw::midi::tick_t tick) const = 0;
    virtual kfw::midi::tick_t beatToRawTick(int measureIndex, int beatIndex) const = 0;

    virtual double tempoMultiplier() const = 0;
    virtual void setTempoMultiplier(double multiplier) = 0;

    virtual void addSoundFlags(const std::vector<kn::engraving::StaffText*>& staffTextList) = 0;
    virtual void removeSoundFlags(const engraving::InstrumentTrackIdSet& trackIdSet) = 0;
    virtual bool hasSoundFlags(const engraving::InstrumentTrackIdSet& trackIdSet) = 0;
};

using INotationPlaybackPtr = std::shared_ptr<INotationPlayback>;
}

#endif // KN_NOTATION_INOTATIONPLAYBACK_H
