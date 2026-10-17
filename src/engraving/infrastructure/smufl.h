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
#pragma once

#include <array>
#include <map>

#include "types/string.h"

#include "../types/symid.h"

namespace kn::engraving {
class Smufl
{
public:

    static bool init();

    struct Code {
        char32_t smuflCode = 0;
        char32_t musicSymBlockCode = 0;

        bool isValid() const { return smuflCode != 0 || musicSymBlockCode != 0; }
    };

    static const Code code(SymId id);
    static char32_t smuflCode(SymId id);

    static const std::map<kfw::String, std::vector<SymId> >& smuflRanges();
    static constexpr const char* SMUFL_ALL_SYMBOLS = "All symbols";

private:

    static bool loadGlyphNamesJson();
    static std::map<kfw::String, std::vector<SymId> > readSmuflRangesJson();

    static std::array<Code, size_t(SymId::lastSym) + 1> s_symIdCodes;
};
}
