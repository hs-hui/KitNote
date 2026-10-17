/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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

#ifndef KN_ENGRAVING_CHORDARTICULATIONSRENDERER_H
#define KN_ENGRAVING_CHORDARTICULATIONSRENDERER_H

#include "renderbase.h"

namespace kn::engraving {
class ChordArticulationsRenderer : public RenderBase<ChordArticulationsRenderer>
{
public:
    static const kfw::mpe::ArticulationTypeSet& supportedTypes();

    static void doRender(const EngravingItem* item, const kfw::mpe::ArticulationType type, const RenderingContext& ctx,
                         kfw::mpe::PlaybackEventList& result);

private:
    static void renderChord(const Chord* chord, const RenderingContext& ctx, kfw::mpe::PlaybackEventList& result);
    static void renderChordWithGraceChords(const Chord* chord, const kfw::mpe::ArticulationType type, const RenderingContext& ctx,
                                           kfw::mpe::PlaybackEventList& result);
    static bool renderChordArticulations(const Chord* chord, const RenderingContext& ctx, kfw::mpe::PlaybackEventList& result);
};
}

#endif // KN_ENGRAVING_CHORDARTICULATIONSRENDERER_H
