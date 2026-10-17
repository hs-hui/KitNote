/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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
#ifndef KN_ENGRAVING_SEGMENTLAYOUT_DEV_H
#define KN_ENGRAVING_SEGMENTLAYOUT_DEV_H

#include "dom/segment.h"
#include "layoutcontext.h"

namespace kn::engraving::rendering::score {
class SegmentLayout
{
public:

    static void layoutMeasureIndependentElements(const Segment& segment, track_idx_t track, const LayoutContext& ctx);
    static void setChordMag(const Staff* staff, const Segment& segment, track_idx_t startTrack, track_idx_t endTrack,
                            const LayoutConfiguration& conf);
    static void checkStaffMoveValidity(const Segment& segment, track_idx_t startTrack, track_idx_t endTrack);
    static void layoutChordDrumset(const Staff* staff, const Segment& segment, track_idx_t startTrack, track_idx_t endTrack,
                                   const LayoutConfiguration& conf);

    static void computeChordsUp(const Segment& segment, track_idx_t startTrack, track_idx_t endTrack, const LayoutContext& ctx);

    static void layoutChordsStem(const Segment& segment, track_idx_t startTrack, track_idx_t endTrack, const LayoutContext& ctx);
};
}

#endif // KN_ENGRAVING_SEGMENTLAYOUT_DEV_H
