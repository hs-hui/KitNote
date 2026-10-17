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
#include "notationplaybackstub.h"

using namespace kn;
using namespace kn::notation;
using namespace kn::engraving;
using namespace kfw;
using namespace kfw::midi;
using namespace kfw::async;

NotationPlaybackStub::NotationPlaybackStub()
{
}

void NotationPlaybackStub::init()
{
}

void NotationPlaybackStub::reload()
{
}

void NotationPlaybackStub::setSendEventsOnScoreChange(const InstrumentTrackId&, bool)
{
}

void NotationPlaybackStub::sendEventsForChangedTracks()
{
}

kfw::async::Channel<InstrumentTrackIdSet> NotationPlaybackStub::tracksDataChanged() const
{
    return {};
}

const engraving::InstrumentTrackId& NotationPlaybackStub::metronomeTrackId() const
{
    static const engraving::InstrumentTrackId dummy;
    return dummy;
}

engraving::InstrumentTrackId NotationPlaybackStub::chordSymbolsTrackId(const ID&) const
{
    return {};
}

bool NotationPlaybackStub::isChordSymbolsTrack(const engraving::InstrumentTrackId&) const
{
    return false;
}

const kfw::mpe::PlaybackData& NotationPlaybackStub::trackPlaybackData(const engraving::InstrumentTrackId&) const
{
    static const kfw::mpe::PlaybackData dummy;
    return dummy;
}

void NotationPlaybackStub::triggerEventsForItems(const std::vector<const EngravingItem*>&, kfw::mpe::duration_t, bool)
{
}

void NotationPlaybackStub::triggerMetronome(kfw::midi::tick_t)
{
}

void NotationPlaybackStub::setMetronomeOverride(std::optional<bool>)
{
}

void NotationPlaybackStub::triggerCountIn(kfw::midi::tick_t, kfw::secs_t&)
{
}

void NotationPlaybackStub::triggerControllers(const kfw::mpe::ControllerChangeEventList&, notation::staff_idx_t, int)
{
}

InstrumentTrackIdSet NotationPlaybackStub::existingTrackIdSet() const
{
    return engraving::InstrumentTrackIdSet();
}

kfw::async::Channel<InstrumentTrackId> NotationPlaybackStub::trackAdded() const
{
    return kfw::async::Channel<InstrumentTrackId>();
}

kfw::async::Channel<InstrumentTrackId> NotationPlaybackStub::trackRemoved() const
{
    return kfw::async::Channel<InstrumentTrackId>();
}

kfw::audio::secs_t NotationPlaybackStub::totalPlayTime() const
{
    return kfw::audio::secs_t();
}

kfw::async::Channel<kfw::audio::secs_t> NotationPlaybackStub::totalPlayTimeChanged() const
{
    return kfw::async::Channel<kfw::audio::secs_t>();
}

kfw::audio::secs_t NotationPlaybackStub::playedTickToSec(tick_t) const
{
    return kfw::audio::secs_t();
}

tick_t NotationPlaybackStub::secToPlayedTick(kfw::audio::secs_t) const
{
    return 0;
}

tick_t NotationPlaybackStub::secToTick(kfw::audio::secs_t) const
{
    return 0;
}

RetVal<kfw::midi::tick_t> NotationPlaybackStub::playPositionTickByRawTick(kfw::midi::tick_t) const
{
    return RetVal<kfw::midi::tick_t>::make_ok(0);
}

RetVal<kfw::midi::tick_t> NotationPlaybackStub::playPositionTickByElement(const EngravingItem*) const
{
    return RetVal<kfw::midi::tick_t>::make_ok(0);
}

void NotationPlaybackStub::addLoopBoundary(LoopBoundaryType, tick_t)
{
}

void NotationPlaybackStub::setLoopBoundariesEnabled(bool)
{
}

bool NotationPlaybackStub::isLoopEnabled() const
{
    return false;
}

const LoopBoundaries& NotationPlaybackStub::loopBoundaries() const
{
    static const LoopBoundaries dummy;
    return dummy;
}

Notification NotationPlaybackStub::loopBoundariesChanged() const
{
    return Notification();
}

const Tempo& NotationPlaybackStub::multipliedTempo(tick_t) const
{
    static const Tempo dummy;
    return dummy;
}

MeasureBeat NotationPlaybackStub::beat(tick_t) const
{
    return MeasureBeat();
}

tick_t NotationPlaybackStub::beatToRawTick(int, int) const
{
    return 0;
}

double NotationPlaybackStub::tempoMultiplier() const
{
    return 0;
}

void NotationPlaybackStub::setTempoMultiplier(double)
{
}

void NotationPlaybackStub::addSoundFlags(const std::vector<StaffText*>&)
{
}

void NotationPlaybackStub::removeSoundFlags(const InstrumentTrackIdSet&)
{
}

bool NotationPlaybackStub::hasSoundFlags(const engraving::InstrumentTrackIdSet&)
{
    return false;
}
