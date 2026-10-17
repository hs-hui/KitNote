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

#pragma once

#include "project/irecentfilescontroller.h"

namespace kn::project {
class RecentFilesControllerStub : public IRecentFilesController
{
public:
    RecentFilesControllerStub() = default;

    const RecentFilesList& recentFilesList() const override;
    kfw::async::Notification recentFilesListChanged() const override;

    void prependRecentFile(const RecentFile& file) override;
    void moveRecentFile(const kfw::io::path_t& before, const RecentFile& after) override;
    void clearRecentFiles() override;

    kfw::async::Promise<QPixmap> thumbnail(const kfw::io::path_t& filePath) const override;
};
}
