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

#include <engraving/types/pitchvalue.h>
#include <engraving/types/types.h>

#include "benddatacontext.h"
#include "splitchord/benddatacollectorsplitchord.h"

namespace kn::engraving {
class Note;
class Chord;
class Score;
}

namespace kn::iex::guitarpro {
class BendDataCollector
{
public:

    BendDataCollector(kn::engraving::Score* score);
    void storeBendData(const kn::engraving::Note* note, const kn::engraving::PitchValues& pitchValues);
    BendDataContext collectBendDataContext();

private:
    kn::engraving::Score* m_score = nullptr;
    std::unordered_map<kn::engraving::track_idx_t,
                       std::map<kn::engraving::Fraction,
                                std::unordered_map<const kn::engraving::Note*, ImportedBendInfo> > > m_bendInfoForNote;

    std::unordered_map<kn::engraving::track_idx_t,
                       std::map<kn::engraving::Fraction, tied_chords_bend_data_chunk_t> > m_regroupedDataByTiedChords;

    std::unique_ptr<BendDataCollectorSplitChord> m_bendDataCollectorSplitChord;

    // converts m_bendInfoForNote to m_regroupedData, leaves m_bendInfoForNote empty
    // m_bendInfoForNote is needed to store separate bend data for each note, while m_regroupedData stores in format, comfortoble for import
    void regroupBendDataByTiedChords();
    void fillBendDataContext(BendDataContext& bendDataCtx);

    // if first chord in chunk has more segments than tied notes, move segments forward to other notes
    void moveSegmentsToTiedNotes(tied_chords_bend_data_chunk_t& dataChunk, ImportedBendInfo& dataForFirstNote, size_t noteIdx);
};
} // kn::iex::guitarpro
