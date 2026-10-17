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
#ifndef KN_NOTATION_NOTATIONPLAYBACK_H
#define KN_NOTATION_NOTATIONPLAYBACK_H

#include "modularity/ioc.h"
#include "async/asyncable.h"
#include "engraving/playback/playbackmodel.h"

#include "../inotationplayback.h"
#include "igetscore.h"
#include "inotationconfiguration.h"

namespace kn::engraving {
class Score;
}

namespace kn::notation {
class NotationPlayback : public INotationPlayback, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> configuration;

public:
    NotationPlayback(IGetScore* getScore, kfw::async::Notification notationChanged, const kfw::modularity::ContextPtr& iocCtx);

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

private:
    engraving::Score* score() const;

    void addLoopIn(int tick);
    void addLoopOut(int tick);
    void updateLoopBoundaries();
    void updateTotalPlayTime();

    bool doAddSoundFlag(kn::engraving::StaffText* staffText);

    std::vector<kn::engraving::StaffText*> collectStaffText(const kn::engraving::InstrumentTrackIdSet& trackIdSet,
                                                            bool withSoundFlags) const;

    IGetScore* m_getScore = nullptr;

    kfw::async::Notification m_notationChanged;

    LoopBoundaries m_loopBoundaries;
    kfw::async::Notification m_loopBoundariesChanged;

    kfw::audio::secs_t m_totalPlayTime = 0;
    kfw::async::Channel<kfw::audio::secs_t> m_totalPlayTimeChanged;

    mutable Tempo m_currentTempo;

    mutable engraving::PlaybackModel m_playbackModel;
    std::optional<bool> m_metronomeOverride; // KitNote
};
}

#endif // KN_NOTATION_NOTATIONPLAYBACK_H
