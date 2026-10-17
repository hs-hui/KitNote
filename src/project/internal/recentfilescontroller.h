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

#include "irecentfilescontroller.h"

#include <mutex>
#include <map>

#include "async/asyncable.h"
#include "async/promise.h"

#include "modularity/ioc.h"
#include "iprojectconfiguration.h"
#include "imscmetareader.h"
#include "io/ifilesystem.h"
#include "multiwindows/imultiwindowsprovider.h"

namespace kn::project {
class RecentFilesController : public IRecentFilesController, public kfw::async::Asyncable
{
    kfw::GlobalThreadSafeInject<IProjectConfiguration> configuration;
    kfw::GlobalThreadSafeInject<kfw::io::IFileSystem> fileSystem;
    kfw::GlobalThreadSafeInject<kfw::mi::IMultiWindowsProvider> multiwindowsProvider;
    kfw::GlobalThreadSafeInject<IMscMetaReader> mscMetaReader;

public:
    void init();

    const RecentFilesList& recentFilesList() const override;
    kfw::async::Notification recentFilesListChanged() const override;

    void prependRecentFile(const RecentFile& file) override;
    void moveRecentFile(const kfw::io::path_t& before, const RecentFile& after) override;
    void clearRecentFiles() override;

    kfw::async::Promise<QPixmap> thumbnail(const kfw::io::path_t& file) const override;

protected:
    virtual void prependPlatformRecentFile(const kfw::io::path_t& path);
    virtual void clearPlatformRecentFiles();

private:
    void loadRecentFilesList();
    void removeNonexistentFiles();
    void setRecentFilesList(const RecentFilesList& list, bool saveAndNotify);
    void saveRecentFilesList();

    void cleanUpThumbnailCache(const RecentFilesList& files);

    mutable bool m_dirty = true;
    mutable RecentFilesList m_recentFilesList;
    kfw::async::Notification m_recentFilesListChanged;
    mutable bool m_isSaving = false;

    struct CachedThumbnail {
        QPixmap thumbnail;
        kfw::DateTime lastModified;
    };

    mutable std::mutex m_thumbnailCacheMutex;
    mutable std::map<kfw::io::path_t, CachedThumbnail> m_thumbnailCache;
};
}
