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
#ifndef KN_NOTATION_NOTATIONERRORS_H
#define KN_NOTATION_NOTATIONERRORS_H

#include "types/ret.h"
#include "translation.h"

namespace kn::notation {
enum class Err {
    Undefined       = int(kfw::Ret::Code::Undefined),
    NoError         = int(kfw::Ret::Code::Ok),
    UnknownError    = int(kfw::Ret::Code::NotationFirst),

    // selection
    SelectCompleteTupletOrTremolo = 1050,
    EmptySelection,
};

inline kfw::Ret make_ret(Err err)
{
    std::string text;

    switch (err) {
    case Err::UnknownError:
        text = kfw::trc("notation", "Unknown error");
        break;
    case Err::SelectCompleteTupletOrTremolo:
        text = kfw::trc("notation", "Please select the complete tuplet or tremolo and retry");
        break;
    case Err::EmptySelection:
        text = kfw::trc("notation", "The selection is empty");
        break;
    case Err::Undefined:
    case Err::NoError:
        break;
    }

    return kfw::Ret(static_cast<int>(err), text);
}
}

#endif // KN_NOTATION_NOTATIONERRORS_H
