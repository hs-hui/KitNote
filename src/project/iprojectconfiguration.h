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
#ifndef KN_PROJECT_IPROJECTCONFIGURATION_H
#define KN_PROJECT_IPROJECTCONFIGURATION_H

#include <QStringList>
#include <QColor>

#include "modularity/imoduleinterface.h"
#include "async/channel.h"
#include "async/notification.h"
#include "io/path.h"
#include "types/bytearray.h"

#include "inotationproject.h"
#include "types/projecttypes.h"

namespace kn::project {
class IProjectConfiguration : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IProjectConfiguration)

public:
    virtual ~IProjectConfiguration() = default;

    virtual kfw::io::path_t recentFilesJsonPath() const = 0;
    virtual kfw::ByteArray compatRecentFilesData() const = 0;

    virtual kfw::io::path_t myFirstProjectPath() const = 0;

    virtual kfw::io::paths_t availableTemplateDirs() const = 0;
    virtual kfw::io::path_t templateCategoriesJsonPath(const kfw::io::path_t& templatesDir) const = 0;

    virtual kfw::io::path_t userTemplatesPath() const = 0;
    virtual void setUserTemplatesPath(const kfw::io::path_t& path) = 0;
    virtual kfw::async::Channel<kfw::io::path_t> userTemplatesPathChanged() const = 0;

    virtual kfw::io::path_t lastOpenedProjectsPath() const = 0;
    virtual void setLastOpenedProjectsPath(const kfw::io::path_t& path) = 0;

    virtual kfw::io::path_t lastSavedProjectsPath() const = 0;
    virtual void setLastSavedProjectsPath(const kfw::io::path_t& path) = 0;

    virtual kfw::io::path_t userProjectsPath() const = 0;
    virtual void setUserProjectsPath(const kfw::io::path_t& path) = 0;
    virtual kfw::async::Channel<kfw::io::path_t> userProjectsPathChanged() const = 0;
    virtual kfw::io::path_t defaultUserProjectsPath() const = 0;

    //! NOTE: best-guess directory to start browsing from in an "open file" dialog
    virtual kfw::io::path_t defaultOpenProjectsPath() const = 0;




    virtual kfw::io::path_t defaultSavingFilePath(INotationProjectPtr project, const std::string& filenameAddition = "",
                                                   const std::string& suffix = "") const = 0;

    virtual SaveLocationType lastUsedSaveLocationType() const = 0;
    virtual void setLastUsedSaveLocationType(SaveLocationType type) = 0;




    enum class HomeScoresPageViewType {
        Grid,
        List
    };

    virtual HomeScoresPageViewType homeScoresPageViewType() const = 0;
    virtual void setHomeScoresPageViewType(HomeScoresPageViewType type) = 0;

    virtual QColor templatePreviewBackgroundColor() const = 0;
    virtual kfw::async::Notification templatePreviewBackgroundChanged() const = 0;

    enum class PreferredScoreCreationMode {
        FromInstruments,
        FromTemplate
    };

    virtual PreferredScoreCreationMode preferredScoreCreationMode() const = 0;
    virtual void setPreferredScoreCreationMode(PreferredScoreCreationMode mode) = 0;

    virtual MigrationOptions migrationOptions(MigrationType type) const = 0;
    virtual void setMigrationOptions(MigrationType type, const MigrationOptions& opt, bool persistent = true) = 0;

    virtual bool isAutoSaveEnabled() const = 0;
    virtual void setAutoSaveEnabled(bool enabled) = 0;
    virtual kfw::async::Channel<bool> autoSaveEnabledChanged() const = 0;

    virtual int autoSaveIntervalMinutes() const = 0;
    virtual void setAutoSaveInterval(int minutes) = 0;
    virtual kfw::async::Channel<int> autoSaveIntervalChanged() const = 0;




    virtual kfw::io::path_t newProjectTemporaryPath() const = 0;

    virtual bool isAccessibleEnabled() const = 0;

    virtual bool shouldDestinationFolderBeOpenedOnExport() const = 0;
    virtual void setShouldDestinationFolderBeOpenedOnExport(bool shouldDestinationFolderBeOpenedOnExport) = 0;









    virtual kfw::io::path_t projectBackupPath(const kfw::io::path_t& projectPath) const = 0;


    virtual bool disableVersionChecking() const = 0;
    virtual void setDisableVersionChecking(bool disable) = 0;

    virtual bool createBackupBeforeSaving() const = 0;
    virtual void setCreateBackupBeforeSaving(bool create) = 0;



    virtual std::string uniqueFileNameAddition(const kfw::io::path_t& filename, const kfw::io::path_t& folderPath,
                                               const std::string& suffix = std::string()) const = 0;
};
}

#endif // KN_PROJECT_IPROJECTCONFIGURATION_H
