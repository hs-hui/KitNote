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

#include "project/iprojectconfiguration.h"

namespace kn::project {
class ProjectConfigurationStub : public IProjectConfiguration
{
public:
    ProjectConfigurationStub() = default;

    kfw::io::path_t recentFilesJsonPath() const override;
    kfw::ByteArray compatRecentFilesData() const override;

    kfw::io::path_t myFirstProjectPath() const override;

    kfw::io::paths_t availableTemplateDirs() const override;
    kfw::io::path_t templateCategoriesJsonPath(const kfw::io::path_t& templatesDir) const override;

    kfw::io::path_t userTemplatesPath() const override;
    void setUserTemplatesPath(const kfw::io::path_t& path) override;
    kfw::async::Channel<kfw::io::path_t> userTemplatesPathChanged() const override;

    kfw::io::path_t lastOpenedProjectsPath() const override;
    void setLastOpenedProjectsPath(const kfw::io::path_t& path) override;

    kfw::io::path_t lastSavedProjectsPath() const override;
    void setLastSavedProjectsPath(const kfw::io::path_t& path) override;

    kfw::io::path_t userProjectsPath() const override;
    void setUserProjectsPath(const kfw::io::path_t& path) override;
    kfw::async::Channel<kfw::io::path_t> userProjectsPathChanged() const override;
    kfw::io::path_t defaultUserProjectsPath() const override;
    kfw::io::path_t defaultOpenProjectsPath() const override;



    kfw::io::path_t defaultSavingFilePath(INotationProjectPtr project, const std::string& filenameAddition = "",
                                           const std::string& suffix = "") const override;

    SaveLocationType lastUsedSaveLocationType() const override;
    void setLastUsedSaveLocationType(SaveLocationType type) override;




    HomeScoresPageViewType homeScoresPageViewType() const override;
    void setHomeScoresPageViewType(HomeScoresPageViewType type) override;

    QColor templatePreviewBackgroundColor() const override;
    kfw::async::Notification templatePreviewBackgroundChanged() const override;

    PreferredScoreCreationMode preferredScoreCreationMode() const override;
    void setPreferredScoreCreationMode(PreferredScoreCreationMode mode) override;

    MigrationOptions migrationOptions(MigrationType type) const override;
    void setMigrationOptions(MigrationType type, const MigrationOptions& opt, bool persistent = true) override;

    bool isAutoSaveEnabled() const override;
    void setAutoSaveEnabled(bool enabled) override;
    kfw::async::Channel<bool> autoSaveEnabledChanged() const override;

    int autoSaveIntervalMinutes() const override;
    void setAutoSaveInterval(int minutes) override;
    kfw::async::Channel<int> autoSaveIntervalChanged() const override;




    kfw::io::path_t newProjectTemporaryPath() const override;

    bool isAccessibleEnabled() const override;

    bool shouldDestinationFolderBeOpenedOnExport() const override;
    void setShouldDestinationFolderBeOpenedOnExport(bool shouldDestinationFolderBeOpenedOnExport) override;









    kfw::io::path_t projectBackupPath(const kfw::io::path_t& projectPath) const override;


    bool disableVersionChecking() const override;
    void setDisableVersionChecking(bool disable) override;

    bool createBackupBeforeSaving() const override;
    void setCreateBackupBeforeSaving(bool create) override;



    std::string uniqueFileNameAddition(const kfw::io::path_t& filename, const kfw::io::path_t& folderPath,
                                       const std::string& suffix = std::string()) const override;
};
}
