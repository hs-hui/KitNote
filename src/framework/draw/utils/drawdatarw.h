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
#ifndef KFW_DRAW_DRAWDATARW_H
#define KFW_DRAW_DRAWDATARW_H

#include "global/io/path.h"
#include "global/types/retval.h"

#include "../types/drawdata.h"

namespace kfw::draw {
class DrawDataRW
{
public:
    DrawDataRW() = default;

    static RetVal<DrawDataPtr> readData(const io::path_t& filePath);
    static Ret writeData(const io::path_t& filePath, const DrawDataPtr& data, bool prettify = true);

    static RetVal<Diff> readDiff(const io::path_t& filePath);
    static Ret writeDiff(const io::path_t& filePath, const Diff& diff);
};
}

#endif // KFW_DRAW_DRAWDATARW_H
