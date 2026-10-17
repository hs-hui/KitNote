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
#include "projectconfigurationstub.h"

using namespace kn::project;

kfw::io::path_t ProjectConfigurationStub::recentFilesJsonPath() const
{
    return kfw::io::path_t();
}

kfw::ByteArray ProjectConfigurationStub::compatRecentFilesData() const
{
    return {};
}

kfw::io::path_t ProjectConfigurationStub::myFirstProjectPath() const
{
    return kfw::io::path_t();
}

kfw::io::paths_t ProjectConfigurationStub::availableTemplateDirs() const
{
    return {};
}

kfw::io::path_t ProjectConfigurationStub::templateCategoriesJsonPath(const kfw::io::path_t&) const
{
    return kfw::io::path_t();
}

kfw::io::path_t ProjectConfigurationStub::userTemplatesPath() const
{
    return kfw::io::path_t();
}

void ProjectConfigurationStub::setUserTemplatesPath(const kfw::io::path_t&)
{
}

kfw::async::Channel<kfw::io::path_t> ProjectConfigurationStub::userTemplatesPathChanged() const
{
    static kfw::async::Channel<kfw::io::path_t> ch;
    return ch;
}

kfw::io::path_t ProjectConfigurationStub::lastOpenedProjectsPath() const
{
    return kfw::io::path_t();
}

void ProjectConfigurationStub::setLastOpenedProjectsPath(const kfw::io::path_t&)
{
}

kfw::io::path_t ProjectConfigurationStub::lastSavedProjectsPath() const
{
    return kfw::io::path_t();
}

void ProjectConfigurationStub::setLastSavedProjectsPath(const kfw::io::path_t&)
{
}

kfw::io::path_t ProjectConfigurationStub::userProjectsPath() const
{
    return kfw::io::path_t();
}

void ProjectConfigurationStub::setUserProjectsPath(const kfw::io::path_t&)
{
}

kfw::async::Channel<kfw::io::path_t> ProjectConfigurationStub::userProjectsPathChanged() const
{
    static kfw::async::Channel<kfw::io::path_t> ch;
    return ch;
}

kfw::io::path_t ProjectConfigurationStub::defaultUserProjectsPath() const
{
    return kfw::io::path_t();
}

kfw::io::path_t ProjectConfigurationStub::defaultOpenProjectsPath() const
{
    return kfw::io::path_t();
}

kfw::io::path_t ProjectConfigurationStub::defaultSavingFilePath(INotationProjectPtr, const std::string&, const std::string&) const
{
    return kfw::io::path_t();
}

SaveLocationType ProjectConfigurationStub::lastUsedSaveLocationType() const
{
    return SaveLocationType::Undefined;
}

void ProjectConfigurationStub::setLastUsedSaveLocationType(SaveLocationType)
{
}

IProjectConfiguration::HomeScoresPageViewType ProjectConfigurationStub::homeScoresPageViewType() const
{
    return IProjectConfiguration::HomeScoresPageViewType::Grid;
}

void ProjectConfigurationStub::setHomeScoresPageViewType(IProjectConfiguration::HomeScoresPageViewType)
{
}

QColor ProjectConfigurationStub::templatePreviewBackgroundColor() const
{
    return QColor();
}

kfw::async::Notification ProjectConfigurationStub::templatePreviewBackgroundChanged() const
{
    static kfw::async::Notification n;
    return n;
}

ProjectConfigurationStub::PreferredScoreCreationMode ProjectConfigurationStub::preferredScoreCreationMode() const
{
    return PreferredScoreCreationMode::FromInstruments;
}

void ProjectConfigurationStub::setPreferredScoreCreationMode(PreferredScoreCreationMode)
{
}

MigrationOptions ProjectConfigurationStub::migrationOptions(MigrationType) const
{
    return MigrationOptions();
}

void ProjectConfigurationStub::setMigrationOptions(MigrationType, const MigrationOptions&, bool)
{
}

bool ProjectConfigurationStub::isAutoSaveEnabled() const
{
    return false;
}

void ProjectConfigurationStub::setAutoSaveEnabled(bool)
{
}

kfw::async::Channel<bool> ProjectConfigurationStub::autoSaveEnabledChanged() const
{
    static kfw::async::Channel<bool> ch;
    return ch;
}

int ProjectConfigurationStub::autoSaveIntervalMinutes() const
{
    return 1;
}

void ProjectConfigurationStub::setAutoSaveInterval(int)
{
}

kfw::async::Channel<int> ProjectConfigurationStub::autoSaveIntervalChanged() const
{
    static kfw::async::Channel<int> ch;
    return ch;
}

kfw::io::path_t ProjectConfigurationStub::newProjectTemporaryPath() const
{
    return kfw::io::path_t();
}

bool ProjectConfigurationStub::isAccessibleEnabled() const
{
    return false;
}

bool ProjectConfigurationStub::shouldDestinationFolderBeOpenedOnExport() const
{
    return false;
}

void ProjectConfigurationStub::setShouldDestinationFolderBeOpenedOnExport(bool)
{
}

kfw::io::path_t ProjectConfigurationStub::projectBackupPath(const kfw::io::path_t&) const
{
    return kfw::io::path_t();
}

bool ProjectConfigurationStub::disableVersionChecking() const
{
    return false;
}

void ProjectConfigurationStub::setDisableVersionChecking(bool)
{
}

bool ProjectConfigurationStub::createBackupBeforeSaving() const
{
    return false;
}

void ProjectConfigurationStub::setCreateBackupBeforeSaving(bool)
{
}

std::string ProjectConfigurationStub::uniqueFileNameAddition(const kfw::io::path_t&, const kfw::io::path_t&, const std::string&) const
{
    return std::string();
}
