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

#include "global/types/string.h"
#include "global/types/val.h"
#include "global/realfn.h"

#include "engraving/types/types.h"

namespace kn::converter {
struct ElementInfo
{
    kn::engraving::ElementType type = kn::engraving::ElementType::INVALID;
    kfw::String name;
    std::map<kfw::String, kfw::Val> data;

    struct Note {
        kfw::String name;
        std::map<kfw::String, kfw::Val> data;
    };

    using NoteList = std::vector<Note>;
    NoteList notes;

    struct Duration {
        kfw::String name;
        uint8_t dots = 0;
    } duration;

    struct Location {
        kn::engraving::staff_idx_t staffIdx = kfw::nidx;
        kn::engraving::voice_idx_t voiceIdx = kfw::nidx;
        size_t measureIdx = kfw::nidx;
        float beat = -1.f;

        bool operator==(const Location& l) const
        {
            return staffIdx == l.staffIdx
                   && voiceIdx == l.voiceIdx
                   && measureIdx == l.measureIdx
                   && kfw::RealIsEqual(beat, l.beat);
        }
    } start, end;
};

// Instrument -> Elements
using ElementInfoList = std::vector<ElementInfo>;
using ElementMap = std::map<kn::engraving::InstrumentTrackId, ElementInfoList>;
}
