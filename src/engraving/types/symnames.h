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
#include "types/translatablestring.h"

#include "symid.h"

namespace kn::engraving {
struct SymNames {
    static kfw::AsciiStringView nameForSymId(SymId id);
    static const kfw::TranslatableString& userNameForSymId(SymId id);
    static kfw::String translatedUserNameForSymId(SymId id);

    static SymId symIdByName(const kfw::AsciiStringView& name, SymId def = SymId::noSym);
    static SymId symIdByName(const kfw::String& name, SymId def = SymId::noSym);
    static SymId symIdByOldName(const kfw::AsciiStringView& oldName);
    static SymId symIdByUserName(const kfw::String& userName);

private:
    static void loadNameToSymIdHash();

    static const std::array<kfw::AsciiStringView, size_t(SymId::lastSym) + 1> s_symNames;
    static const std::array<kfw::TranslatableString, size_t(SymId::lastSym) + 1> s_symUserNames;

    //! Will be initialized when first used
    static std::map<kfw::AsciiStringView, SymId> s_nameToSymIdHash;
    static const std::map<kfw::AsciiStringView, SymId> s_oldNameToSymIdHash;
};
}
