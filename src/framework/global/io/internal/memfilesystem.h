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

#include <map>

#include "../ifilesystem.h"

namespace kfw::io {
class MemFileSystem : public kfw::io::IFileSystem
{
public:
    MemFileSystem() = default;

    kfw::Ret exists(const kfw::io::path_t& path) const override;
    kfw::Ret remove(const kfw::io::path_t& path, bool onlyIfEmpty = false) override;  // remove file or dir
    kfw::Ret clear(const kfw::io::path_t& path) override;  // clear dir
    kfw::Ret copy(const kfw::io::path_t& src, const kfw::io::path_t& dst, bool replace = false) override;
    kfw::Ret move(const kfw::io::path_t& src, const kfw::io::path_t& dst, bool replace = false) override;

    kfw::Ret makePath(const kfw::io::path_t& path) const override;
    kfw::Ret makeLink(const kfw::io::path_t& targetPath, const kfw::io::path_t& linkPath) const override;

    kfw::io::EntryType entryType(const kfw::io::path_t& path) const override;

    kfw::RetVal<uint64_t> fileSize(const kfw::io::path_t& path) const override;

    kfw::RetVal<kfw::io::paths_t> scanFiles(const kfw::io::path_t& rootDir, const std::vector<std::string>& filters,
                                              kfw::io::ScanMode mode = kfw::io::ScanMode::FilesInCurrentDirAndSubdirs) const override;

    void setAttribute(const kfw::io::path_t& path, Attribute attribute) const override;
    bool setPermissionsAllowedForAll(const kfw::io::path_t& path) const override;

    kfw::RetVal<kfw::ByteArray> readFile(const kfw::io::path_t& filePath) const override;
    kfw::Ret readFile(const kfw::io::path_t& filePath, kfw::ByteArray& data) const override;
    kfw::Ret writeFile(const kfw::io::path_t& filePath, const kfw::ByteArray& data) override;

    kfw::RetVal<StreamId> openStream(const kfw::io::path_t& filePath, OpenMode mode) override;
    kfw::Ret writeToStream(StreamId fileId, const kfw::ByteArray& data, uint64_t offset = STREAM_POS_CURRENT) override;
    kfw::Ret closeStream(StreamId fileId) override;

    //! NOTE File info
    kfw::io::path_t canonicalFilePath(const kfw::io::path_t& filePath) const override;
    kfw::io::path_t absolutePath(const kfw::io::path_t& filePath) const override;
    kfw::io::path_t absoluteFilePath(const kfw::io::path_t& filePath) const override;
    kfw::DateTime birthTime(const kfw::io::path_t& filePath) const override;
    kfw::DateTime lastModified(const kfw::io::path_t& filePath) const override;
    kfw::Ret isWritable(const kfw::io::path_t& filePath) const override;

private:

    struct OpenFileState {
        kfw::io::path_t path;
        size_t pos = 0;
    };

    std::map<kfw::io::path_t, kfw::ByteArray> m_files;
    std::map<StreamId, OpenFileState> m_openStates;
    StreamId m_nextStreamId = 0;
};
}
