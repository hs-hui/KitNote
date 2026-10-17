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

#include "mpe/events.h"

#include "renderingcontext.h"

namespace kn::engraving {
class Chord;
class Score;
class Note;

class PlaybackEventsRenderer
{
public:
    PlaybackEventsRenderer() = default;

    void render(const EngravingItem* item, const int tickPositionOffset, const kfw::mpe::ArticulationsProfilePtr profile,
                const PlaybackContextPtr playbackCtx, kfw::mpe::PlaybackEventsMap& result) const;

    void render(const EngravingItem* item, const kfw::mpe::timestamp_t actualTimestamp, const kfw::mpe::duration_t actualDuration,
                const kfw::mpe::dynamic_level_t actualDynamicLevel, const PlaybackContextPtr playbackCtx,
                const kfw::mpe::ArticulationsProfilePtr profile, kfw::mpe::PlaybackEventsMap& result) const;

    void renderChordSymbol(const Harmony* chordSymbol, const int ticksPositionOffset, const kfw::mpe::ArticulationsProfilePtr profile,
                           const PlaybackContextPtr playbackCtx, kfw::mpe::PlaybackEventsMap& result) const;
    void renderChordSymbol(const Harmony* chordSymbol, const kfw::mpe::timestamp_t actualTimestamp,
                           const kfw::mpe::duration_t actualDuration, const kfw::mpe::dynamic_level_t actualDynamicLevel,
                           const kfw::mpe::ArticulationsProfilePtr profile, kfw::mpe::PlaybackEventsMap& result) const;

    void renderMetronome(const Score* score, const Measure* measure, const int ticksPositionOffset,
                         const kfw::mpe::ArticulationsProfilePtr profile, kfw::mpe::PlaybackEventsMap& result) const;

    void renderMetronome(const Score* score, const int tick, const kfw::mpe::timestamp_t actualTimestamp,
                         const kfw::mpe::ArticulationsProfilePtr profile, kfw::mpe::PlaybackEventsMap& result) const;

    void renderCountIn(const Score* score, const int tick, const kfw::mpe::timestamp_t actualTimestamp,
                       const kfw::mpe::ArticulationsProfilePtr profile, kfw::mpe::PlaybackEventsMap& result,
                       kfw::mpe::duration_t& countInDuration) const;

private:
    void renderNoteEvents(const Chord* chord, const int tickPositionOffset, const kfw::mpe::ArticulationsProfilePtr profile,
                          const PlaybackContextPtr playbackCtx, kfw::mpe::PlaybackEventsMap& result) const;

    void renderFixedNoteEvent(const Note* note, const kfw::mpe::timestamp_t actualTimestamp, const kfw::mpe::duration_t actualDuration,
                              const kfw::mpe::dynamic_level_t actualDynamicLevel, const PlaybackContextPtr playbackCtx,
                              const kfw::mpe::ArticulationsProfilePtr profile, kfw::mpe::PlaybackEventList& result) const;
};
}
