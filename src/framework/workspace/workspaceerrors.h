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
#ifndef KFW_WORKSPACE_WORKSPACEERRORS_H
#define KFW_WORKSPACE_WORKSPACEERRORS_H

#include "types/ret.h"

namespace kfw::workspace {
enum class Err {
    Undefined       = int(Ret::Code::Undefined),
    NoError         = int(Ret::Code::Ok),
    UnknownError    = int(Ret::Code::WorkspaceFirst),

    NotLoaded       = 1502,

    FailedPack      = 1510,
    FailedUnPack    = 1511,
};

inline Ret make_ret(Err e)
{
    return Ret(static_cast<int>(e));
}
}

#endif // KFW_WORKSPACE_WORKSPACEERRORS_H
