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

#include <engraving/types/types.h>

// if SPLIT_CHORD_DURATIONS is defined: chord with bend is split to several chords, tied chords may be removed or replaced according to new ticks
// otherwise - "grace note after" is placed where the "small note head" was in guitar pro, and normal note is placed where normal note was in guitar pro
// #define SPLIT_CHORD_DURATIONS 1

#include "guitarbendimporttypes.h"
#include "splitchord/benddatacontextsplitchord.h"

namespace kn::iex::guitarpro {
struct BendDataContext {
    std::unordered_map<kn::engraving::track_idx_t, std::map<kn::engraving::Fraction, grace_bend_data_map_t > > graceAfterBendData;
    std::unordered_map<kn::engraving::track_idx_t, std::map<kn::engraving::Fraction, bend_data_map_t > > tiedNotesBendsData;
    std::unordered_map<kn::engraving::track_idx_t, std::map<kn::engraving::Fraction, bend_data_map_t > > prebendData;
    std::unordered_map<kn::engraving::track_idx_t, std::map<kn::engraving::Fraction, bend_data_map_t > > slightBendData;
    std::unique_ptr<BendDataContextSplitChord> splitChordCtx;
};
} // kn::iex::guitarpro
