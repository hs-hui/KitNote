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

#include "types/ret.h"

namespace kn::converter {
enum class Err {
    Undefined       = int(kfw::Ret::Code::Undefined),
    NoError         = int(kfw::Ret::Code::Ok),
    UnknownError    = int(kfw::Ret::Code::ConverterFirst), // 1300

    BatchJobFileFailedOpen = 1301,
    BatchJobFileFailedParse = 1302,

    ConvertFailed = 1303,
    TransposeFailed = 1304,

    ConvertTypeUnknown = 1310,
    InvalidTransposeOptions = 1311,
    InvalidScoreRegion = 1312,

    InFileFailedLoad = 1320,

    OutFileFailedOpen = 1330,
    OutFileFailedWrite = 1331,

    TracksDiffFileFailedOpen = 1332,
};

inline kfw::Ret make_ret(Err e)
{
    return kfw::Ret(static_cast<int>(e));
}

inline kfw::Ret make_ret(Err e, const std::string& text)
{
    return kfw::Ret(static_cast<int>(e), text);
}
}
