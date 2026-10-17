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
#pragma once

#include "../inotationplayback.h"

namespace kn::notation {
class NotationPlaybackStub : public INotationPlayback
{
public:
    NotationPlaybackStub();

    void init() override;
    void reload() override;

    void setSendEventsOnScoreChange(const InstrumentTrackId& trackId, bool send) override;
    void sendEventsForChangedTracks() override;

    kfw::async::Channel<InstrumentTrackIdSet> tracksDataChanged() const override;

    const engraving::InstrumentTrackId& metronomeTrackId() const override;
    engraving::InstrumentTrackId chordSymbolsTrackId(const kfw::ID& partId) const override;
    bool isChordSymbolsTrack(const engraving::InstrumentTrackId& trackId) const override;

    const kfw::mpe::PlaybackData& trackPlaybackData(const engraving::InstrumentTrackId& trackId) const override;

    void triggerEventsForItems(const std::vector<const EngravingItem*>& items, kfw::mpe::duration_t duration, bool flushSound) override;
    void triggerMetronome(kfw::midi::tick_t tick) override;
    void setMetronomeOverride(std::optional<bool> enabled) override;
    void triggerCountIn(kfw::midi::tick_t tick, kfw::secs_t& countInDuration) override;
    void triggerControllers(const kfw::mpe::ControllerChangeEventList& list, notation::staff_idx_t staffIdx, int tick) override;

    engraving::InstrumentTrackIdSet existingTrackIdSet() const override;
    kfw::async::Channel<engraving::InstrumentTrackId> trackAdded() const override;
    kfw::async::Channel<engraving::InstrumentTrackId> trackRemoved() const override;

    kfw::audio::secs_t totalPlayTime() const override;
    kfw::async::Channel<kfw::audio::secs_t> totalPlayTimeChanged() const override;

    kfw::audio::secs_t playedTickToSec(kfw::midi::tick_t tick) const override;
    kfw::midi::tick_t secToPlayedTick(kfw::audio::secs_t sec) const override;
    kfw::midi::tick_t secToTick(kfw::audio::secs_t sec) const override;

    kfw::RetVal<kfw::midi::tick_t> playPositionTickByRawTick(kfw::midi::tick_t tick) const override;
    kfw::RetVal<kfw::midi::tick_t> playPositionTickByElement(const EngravingItem* element) const override;

    void addLoopBoundary(LoopBoundaryType boundaryType, kfw::midi::tick_t tick) override;
    void setLoopBoundariesEnabled(bool enabled) override;
    bool isLoopEnabled() const override;
    const LoopBoundaries& loopBoundaries() const override;
    kfw::async::Notification loopBoundariesChanged() const override;

    const Tempo& multipliedTempo(kfw::midi::tick_t tick) const override;
    MeasureBeat beat(kfw::midi::tick_t tick) const override;
    kfw::midi::tick_t beatToRawTick(int measureIndex, int beatIndex) const override;

    double tempoMultiplier() const override;
    void setTempoMultiplier(double multiplier) override;

    void addSoundFlags(const std::vector<kn::engraving::StaffText*>& staffTextList) override;
    void removeSoundFlags(const engraving::InstrumentTrackIdSet& trackIdSet) override;
    bool hasSoundFlags(const engraving::InstrumentTrackIdSet& trackIdSet) override;
};
}
