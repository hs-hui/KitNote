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

#include "localfileinfoprovider.h"

using namespace kfw;
using namespace kn;
using namespace kn::engraving;

LocalFileInfoProvider::LocalFileInfoProvider(const kfw::io::path_t& path)
    : m_path(path)
{
}

bool LocalFileInfoProvider::saved() const
{
    return false;
}

kfw::io::path_t LocalFileInfoProvider::path() const
{
    return io::absolutePath(m_path);
}

kfw::io::path_t LocalFileInfoProvider::fileName(bool includingExtension) const
{
    return io::filename(m_path, includingExtension);
}

kfw::io::path_t LocalFileInfoProvider::absoluteDirPath() const
{
    return io::absoluteDirpath(m_path);
}

String LocalFileInfoProvider::displayName() const
{
    return String::fromUtf8(fileName(/*includingExtension*/ false).c_str());
}

DateTime LocalFileInfoProvider::birthTime() const
{
    return fileSystem()->birthTime(m_path);
}

DateTime LocalFileInfoProvider::lastModified() const
{
    return fileSystem()->lastModified(m_path);
}

bool LocalFileInfoProvider::isNewlyCreated() const
{
    return true;
}
