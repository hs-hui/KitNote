/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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
#include "recentfilescontrollerstub.h"

using namespace kn::project;

const RecentFilesList& RecentFilesControllerStub::recentFilesList() const
{
    static RecentFilesList l;
    return l;
}

kfw::async::Notification RecentFilesControllerStub::recentFilesListChanged() const
{
    static kfw::async::Notification n;
    return n;
}

void RecentFilesControllerStub::prependRecentFile(const RecentFile&)
{
}

void RecentFilesControllerStub::moveRecentFile(const kfw::io::path_t&, const RecentFile&)
{
}

void RecentFilesControllerStub::clearRecentFiles()
{
}

kfw::async::Promise<QPixmap> RecentFilesControllerStub::thumbnail(const kfw::io::path_t&) const
{
    return kfw::async::Promise<QPixmap>([](auto /*resolve*/, auto reject) {
        return reject(int(kfw::Ret::Code::UnknownError), "stub");
    });
}
