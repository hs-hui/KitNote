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

#include "engravingglobals.h"
#include "pitchspelling.h"
#include "../types/types.h"

namespace kn::engraving {
//---------------------------------------------------------
//   NoteVal
///    helper structure
///   \cond PLUGIN_API \private \endcond
//---------------------------------------------------------
struct NoteVal {
    int pitch = -1;
    int velocityOverride = 0;
    int tpc1 = Tpc::TPC_INVALID;
    int tpc2 = Tpc::TPC_INVALID;
    int fret = INVALID_FRET_INDEX;
    int string = INVALID_STRING_INDEX;
    NoteHeadGroup headGroup = NoteHeadGroup::HEAD_NORMAL;

    NoteVal() {}
    NoteVal(int p)
        : pitch(p) {}

    bool operator==(const NoteVal& v) const
    {
        return pitch == v.pitch
               && velocityOverride == v.velocityOverride
               && tpc1 == v.tpc1
               && tpc2 == v.tpc2
               && fret == v.fret
               && string == v.string
               && headGroup == v.headGroup;
    }

    int tpc(bool concertPitch) const
    {
        return concertPitch ? tpc1 : tpc2;
    }

    bool isRest() const
    {
        return pitch == -1;
    }
};

using NoteValList = std::vector<NoteVal>;
}
