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
#ifndef KFW_IO_DIR_H
#define KFW_IO_DIR_H

#include "global/types/retval.h"
#include "global/modularity/ioc.h"

#include "path.h"
#include "ioenums.h"
#include "ifilesystem.h"

namespace kfw::io {
class Dir
{
    static inline GlobalInject<IFileSystem> fileSystem;

public:
    Dir() = default;
    Dir(const path_t& path);

    path_t path() const;
    path_t absolutePath() const;

    bool exists() const;
    Ret removeRecursively();

    Ret mkpath();
    static Ret mkpath(const path_t& path);

    static RetVal<io::paths_t> scanFiles(const io::path_t& rootDir, const std::vector<std::string>& filters,
                                         ScanMode mode = ScanMode::FilesInCurrentDirAndSubdirs);

    static path_t fromNativeSeparators(const path_t& pathName);

private:
    path_t m_path;
};
}

#endif // KFW_IO_DIR_H
