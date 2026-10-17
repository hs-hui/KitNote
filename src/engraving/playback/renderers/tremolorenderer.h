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

#ifndef KN_ENGRAVING_TREMOLORENDERER_H
#define KN_ENGRAVING_TREMOLORENDERER_H

#include "renderbase.h"

namespace kn::engraving {
class TremoloRenderer : public RenderBase<TremoloRenderer>
{
public:
    static const kfw::mpe::ArticulationTypeSet& supportedTypes();

    static void doRender(const EngravingItem* item, const kfw::mpe::ArticulationType preferredType, const RenderingContext& ctx,
                         kfw::mpe::PlaybackEventList& result);

private:
    using TremoloTimeCache = std::unordered_map<const Note*, kfw::mpe::TimestampAndDuration>;

    static int stepDurationTicks(const Chord* chord, int tremoloLines);
    static void buildAndAppendEvents(const Chord* chord, const kfw::mpe::ArticulationType type, const int stepDurationTicks,
                                     const int startTick, const RenderingContext& ctx, TremoloTimeCache& tremoloCache,
                                     kfw::mpe::PlaybackEventList& result);
    static const kfw::mpe::TimestampAndDuration& tremoloTimeAndDuration(const Note* note, const RenderingContext& ctx,
                                                                         TremoloTimeCache& cache);
};
}

#endif // KN_ENGRAVING_TREMOLORENDERER_H
