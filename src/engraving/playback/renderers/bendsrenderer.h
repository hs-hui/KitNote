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

#include "mpe/events.h"

namespace kn::engraving {
class Note;
class GuitarBend;
class Score;
struct RenderingContext;
struct GraceChordCtx;
class BendsRenderer
{
public:
    static bool isMultibendPart(const Note* note);

    static void render(const Note* note, const RenderingContext& ctx, kfw::mpe::PlaybackEventList& result);

private:
    struct BendTimeFactors {
        float startFactor = 0.f;
        float targetFactor = 1.f;
        float endFactor = 1.f;
    };

    using PitchOffsets = std::vector<std::pair<kfw::mpe::timestamp_t, kfw::mpe::pitch_level_t> >;
    using BendTimeFactorMap = std::map<kfw::mpe::timestamp_t, BendTimeFactors>;

    static void renderMultibend(const Note* startNote, const RenderingContext& startNoteCtx, kfw::mpe::PlaybackEventList& result);
    static void renderNote(const Note* note, const RenderingContext& ctx, kfw::mpe::PlaybackEventList& result);
    static void renderGraceNote(const Note* note, const GraceChordCtx& ctx, kfw::mpe::PlaybackEventList& result);
    static void renderSlightBend(const Note* note, const GuitarBend* bend, const RenderingContext& ctx,
                                 kfw::mpe::PlaybackEventList& result);
    static void renderScoop(const Note* note, const GuitarBend* bend, const RenderingContext& ctx, kfw::mpe::PlaybackEventList& result);
    static void renderDip(const Note* note, const GuitarBend* bend, const RenderingContext& ctx, kfw::mpe::PlaybackEventList& result);

    static BendTimeFactors timeFactors(const GuitarBend* bend);

    static RenderingContext buildRenderingContext(const Note* note, const RenderingContext& initialCtx);
    static kfw::mpe::NoteEvent buildBendEvent(const Note* startNote, const RenderingContext& startNoteCtx,
                                               const kfw::mpe::PlaybackEventList& bendNoteEvents, const BendTimeFactorMap& timeFactorMap);
    static kfw::mpe::PitchCurve buildPitchCurve(kfw::mpe::timestamp_t bendStartTime, kfw::mpe::duration_t totalBendDuration,
                                                 const PitchOffsets& pitchOffsets, const BendTimeFactorMap& timeFactorMap);
};
}
