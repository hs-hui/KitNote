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

#ifndef KN_ENGRAVING_ORNAMENTSRENDERER_H
#define KN_ENGRAVING_ORNAMENTSRENDERER_H

#include "renderbase.h"

namespace kn::engraving {
struct DisclosurePattern;
class OrnamentsRenderer : public RenderBase<OrnamentsRenderer>
{
public:
    static const kfw::mpe::ArticulationTypeSet& supportedTypes();

    static void doRender(const EngravingItem* item, const kfw::mpe::ArticulationType preferredType, const RenderingContext& ctx,
                         kfw::mpe::PlaybackEventList& result);

private:
    static void applyTiedNotesDuration(const Note* note, const kfw::mpe::ArticulationType ornamentType, RenderingContext& ctx);

    static void convert(const kfw::mpe::ArticulationType type, const DisclosurePattern& pattern, NominalNoteCtx&& noteCtx,
                        kfw::mpe::PlaybackEventList& result);

    static int alterationsNumberByTempo(const double beatsPerSeconds, const int principalNoteDurationTicks,
                                        const DisclosurePattern& pattern);

    static void createEvents(const kfw::mpe::ArticulationType type, NominalNoteCtx& noteCtx, const int alterationsCount,
                             const int availableDurationTicks, const int overallDurationTicks,
                             const std::vector<kfw::mpe::pitch_level_t>& pitchOffsets, kfw::mpe::PlaybackEventList& result);
};
}

#endif // KN_ENGRAVING_ORNAMENTSRENDERER_H
