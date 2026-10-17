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
#ifndef KFW_GLOBAL_ZIPWRITER_H
#define KFW_GLOBAL_ZIPWRITER_H

#include "io/path.h"
#include "io/iodevice.h"

namespace kfw {
class ZipWriter
{
public:

    explicit ZipWriter(const io::path_t& filePath);
    explicit ZipWriter(io::IODevice* device);
    ~ZipWriter();

    void close();
    bool hasError() const;

    void addFile(const std::string& fileName, const ByteArray& data);

private:

    void flush();

    struct Impl;
    Impl* m_impl = nullptr;
    io::IODevice* m_device = nullptr;
    bool m_selfDevice = false;
};
}

#endif // KFW_GLOBAL_ZIPWRITER_H
