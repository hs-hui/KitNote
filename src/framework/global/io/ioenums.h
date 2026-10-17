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
#ifndef KFW_IO_IOENUMS_H
#define KFW_IO_IOENUMS_H

#include <cstdint>

namespace kfw::io {
using StreamId = int;
static constexpr StreamId INVALID_STREAM_ID = -1;
static constexpr uint64_t STREAM_POS_CURRENT = ~uint64_t(0);

enum class OpenMode {
    WriteOnly,
    Append
};

enum class ScanMode {
    FilesInCurrentDir,
    FoldersInCurrentDir,
    FilesAndFoldersInCurrentDir,
    FilesInCurrentDirAndSubdirs
};

enum class EntryType {
    Undefined = 0,
    File,
    Dir
};
}

#endif // KFW_IO_IOENUMS_H
