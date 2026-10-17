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
#include "projectconfiguration.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include "settings.h"

#include "engraving/infrastructure/mscio.h"
#include "project/internal/notationproject.h"

#include "log.h"

using namespace kn;
using namespace kn::project;
using namespace kn::notation;
using namespace kfw;

static const std::string module_name("project");

static const Settings::Key COMPAT_RECENT_FILES_DATA(module_name, "project/recentList");
static const Settings::Key USER_TEMPLATES_PATH(module_name, "application/paths/myTemplates");
static const Settings::Key LAST_OPENED_PROJECTS_PATH(module_name, "application/paths/lastOpenedProjectsPath");
static const Settings::Key LAST_SAVED_PROJECTS_PATH(module_name, "application/paths/lastSavedProjectsPath");
static const Settings::Key USER_PROJECTS_PATH(module_name, "application/paths/myScores");
static const Settings::Key LAST_USED_SAVE_LOCATION_TYPE(module_name, "project/lastUsedSaveLocationType");
static const Settings::Key HOME_SCORES_PAGE_VIEW_TYPE(module_name, "project/homeScoresPageViewType");
static const Settings::Key PREFERRED_SCORE_CREATION_MODE_KEY(module_name, "project/preferredScoreCreationMode");
static const Settings::Key MIGRATION_OPTIONS(module_name, "project/migration");
static const Settings::Key AUTOSAVE_ENABLED_KEY(module_name, "project/autoSaveEnabled");
static const Settings::Key AUTOSAVE_INTERVAL_KEY(module_name, "project/autoSaveInterval");
static const Settings::Key SHOULD_DESTINATION_FOLDER_BE_OPENED_ON_EXPORT(module_name, "project/shouldDestinationFolderBeOpenedOnExport");
static const Settings::Key DISABLE_VERSION_CHECKING(module_name, "project/disableVersionChecking");
static const Settings::Key CREATE_BACKUP_BEFORE_SAVING(module_name, "project/createBackupBeforeSaving");

static const std::string DEFAULT_FILE_SUFFIX(".kitnote"); // KitNote: own extension by default
static const std::string DEFAULT_FILE_FILTER("*.kitnote");

ProjectConfiguration::ProjectConfiguration(const kfw::modularity::ContextPtr& iocCtx)
    : kfw::Contextable(iocCtx)
{
}

void ProjectConfiguration::init()
{
    settings()->setDefaultValue(USER_TEMPLATES_PATH, Val(globalConfiguration()->userDataPath() + "/Templates"));
    settings()->valueChanged(USER_TEMPLATES_PATH).onReceive(nullptr, [this](const Val& val) {
        m_userTemplatesPathChanged.send(val.toPath());
    });

    settings()->setDefaultValue(USER_PROJECTS_PATH, Val(globalConfiguration()->userDataPath() + "/Scores"));

    settings()->valueChanged(USER_PROJECTS_PATH).onReceive(nullptr, [this](const Val& val) {
        m_userScoresPathChanged.send(val.toPath());
    });

    Val preferredScoreCreationMode = Val(PreferredScoreCreationMode::FromInstruments);
    settings()->setDefaultValue(PREFERRED_SCORE_CREATION_MODE_KEY, preferredScoreCreationMode);

    settings()->setDefaultValue(HOME_SCORES_PAGE_VIEW_TYPE, Val(HomeScoresPageViewType::Grid));

    settings()->setDefaultValue(LAST_USED_SAVE_LOCATION_TYPE, Val(SaveLocationType::Undefined));


    settings()->setDefaultValue(AUTOSAVE_ENABLED_KEY, Val(true));
    settings()->valueChanged(AUTOSAVE_ENABLED_KEY).onReceive(nullptr, [this](const Val& val) {
        m_autoSaveEnabledChanged.send(val.toBool());
    });

    settings()->setDefaultValue(AUTOSAVE_INTERVAL_KEY, Val(5));
    settings()->valueChanged(AUTOSAVE_INTERVAL_KEY).onReceive(nullptr, [this](const Val& val) {
        m_autoSaveIntervalChanged.send(val.toInt());
    });




    settings()->setDefaultValue(SHOULD_DESTINATION_FOLDER_BE_OPENED_ON_EXPORT, Val(false));

    settings()->setDefaultValue(DISABLE_VERSION_CHECKING, Val(false));

    settings()->setDefaultValue(CREATE_BACKUP_BEFORE_SAVING, Val(true));
    settings()->setDescription(CREATE_BACKUP_BEFORE_SAVING, kfw::trc("project",
                                                                      "Create backup of file on disk before saving new changes"));
    settings()->setCanBeManuallyEdited(CREATE_BACKUP_BEFORE_SAVING, true);

    if (!userTemplatesPath().empty()) {
        fileSystem()->makePath(userTemplatesPath());
    }

    if (!userProjectsPath().empty()) {
        fileSystem()->makePath(userProjectsPath());
    }
}

kfw::io::path_t ProjectConfiguration::recentFilesJsonPath() const
{
    return globalConfiguration()->userAppDataPath().appendingComponent("recent_files.json");
}

ByteArray ProjectConfiguration::compatRecentFilesData() const
{
    std::string data = settings()->value(COMPAT_RECENT_FILES_DATA).toString();

    return ByteArray(data.data(), data.size());
}

kfw::io::path_t ProjectConfiguration::myFirstProjectPath() const
{
    return appTemplatesPath() + "/My_First_Score.mscx";
}

kfw::io::path_t ProjectConfiguration::appTemplatesPath() const
{
    return globalConfiguration()->appDataPath() + "/templates";
}

io::paths_t ProjectConfiguration::availableTemplateDirs() const
{
    io::paths_t dirs;

    kfw::io::path_t defaultTemplatesPath = this->appTemplatesPath();
    dirs.push_back(defaultTemplatesPath);

    kfw::io::path_t userTemplatesPath = this->userTemplatesPath();
    if (!userTemplatesPath.empty() && userTemplatesPath != defaultTemplatesPath) {
        dirs.push_back(userTemplatesPath);
    }

    return dirs;
}

kfw::io::path_t ProjectConfiguration::templateCategoriesJsonPath(const kfw::io::path_t& templatesDir) const
{
    return templatesDir + "/categories.json";
}

kfw::io::path_t ProjectConfiguration::userTemplatesPath() const
{
    return settings()->value(USER_TEMPLATES_PATH).toPath();
}

void ProjectConfiguration::setUserTemplatesPath(const kfw::io::path_t& path)
{
    settings()->setSharedValue(USER_TEMPLATES_PATH, Val(path));
}

kfw::async::Channel<kfw::io::path_t> ProjectConfiguration::userTemplatesPathChanged() const
{
    return m_userTemplatesPathChanged;
}

kfw::io::path_t ProjectConfiguration::lastOpenedProjectsPath() const
{
    return settings()->value(LAST_OPENED_PROJECTS_PATH).toPath();
}

void ProjectConfiguration::setLastOpenedProjectsPath(const kfw::io::path_t& path)
{
    settings()->setSharedValue(LAST_OPENED_PROJECTS_PATH, Val(path));
}

kfw::io::path_t ProjectConfiguration::lastSavedProjectsPath() const
{
    return settings()->value(LAST_SAVED_PROJECTS_PATH).toPath();
}

void ProjectConfiguration::setLastSavedProjectsPath(const kfw::io::path_t& path)
{
    settings()->setSharedValue(LAST_SAVED_PROJECTS_PATH, Val(path));
}

kfw::io::path_t ProjectConfiguration::userProjectsPath() const
{
    return settings()->value(USER_PROJECTS_PATH).toPath();
}

void ProjectConfiguration::setUserProjectsPath(const kfw::io::path_t& path)
{
    settings()->setSharedValue(USER_PROJECTS_PATH, Val(path));
}

kfw::async::Channel<kfw::io::path_t> ProjectConfiguration::userProjectsPathChanged() const
{
    return m_userScoresPathChanged;
}

kfw::io::path_t ProjectConfiguration::defaultUserProjectsPath() const
{
    return settings()->defaultValue(USER_PROJECTS_PATH).toPath();
}

kfw::io::path_t ProjectConfiguration::defaultOpenProjectsPath() const
{
    kfw::io::path_t dir = lastOpenedProjectsPath();
    if (dir.empty()) {
        dir = userProjectsPath();
    }

    if (dir.empty()) {
        dir = defaultUserProjectsPath();
    }

    return dir;
}

kfw::io::path_t ProjectConfiguration::defaultSavingFilePath(INotationProjectPtr project, const std::string& filenameAddition,
                                                             const std::string& suffix) const
{
    kfw::io::path_t folderPath;
    kfw::io::path_t filename;
    std::string theSuffix = suffix;
    std::string theFilenameAddition = filenameAddition;

    kfw::io::path_t projectPath = project->path();

    if (project->isNewlyCreated()) {
        if (io::isAbsolute(projectPath)) {
            filename = io::filename(projectPath, false);
            folderPath = io::dirpath(projectPath);
        } else {
            filename = io::filename(projectPath, true);
        }
    } else {
        projectPath = engraving::containerPath(projectPath);
        folderPath = io::dirpath(projectPath);
        filename = io::filename(projectPath, false);

        if (theSuffix.empty()) {
            theSuffix = io::suffix(projectPath);
        }
    }

    if (folderPath.empty()) {
        folderPath = userProjectsPath();
    }

    if (folderPath.empty()) {
        folderPath = lastSavedProjectsPath();
    }

    if (folderPath.empty()) {
        folderPath = defaultUserProjectsPath();
    }

    if (filename.empty()) {
        filename = project->metaInfo().title;
    }

    if (filename.empty()) {
        filename = kfw::qtrc("project", "Untitled");
    }

    if (theSuffix.empty()) {
        theSuffix = DEFAULT_FILE_SUFFIX;
    }

    if (project->isNewlyCreated() && filename.toQString() == NotationProject::scoreDefaultTitle()) {
        theFilenameAddition += uniqueFileNameAddition(filename + theFilenameAddition, folderPath, theSuffix);
    }

    return folderPath
           .appendingComponent(filename + theFilenameAddition)
           .appendingSuffix(theSuffix);
}

std::string ProjectConfiguration::uniqueFileNameAddition(const io::path_t& filename, const io::path_t& folderPath,
                                                         const std::string& suffix) const
{
    // Return a filename addition which would make filename unique if it wasn't already
    const std::string theSuffix = suffix.empty() ? DEFAULT_FILE_SUFFIX : suffix;
    std::string addition;
    int id = 1;
    while (fileSystem()->exists(folderPath
                                .appendingComponent(filename + addition)
                                .appendingSuffix(theSuffix))) {
        addition = " (" + std::to_string(id) + ")";
        id++;
    }
    return addition;
}

SaveLocationType ProjectConfiguration::lastUsedSaveLocationType() const
{
    return settings()->value(LAST_USED_SAVE_LOCATION_TYPE).toEnum<SaveLocationType>();
}

void ProjectConfiguration::setLastUsedSaveLocationType(SaveLocationType type)
{
    settings()->setSharedValue(LAST_USED_SAVE_LOCATION_TYPE, Val(type));
}

IProjectConfiguration::HomeScoresPageViewType ProjectConfiguration::homeScoresPageViewType() const
{
    return settings()->value(HOME_SCORES_PAGE_VIEW_TYPE).toEnum<HomeScoresPageViewType>();
}

void ProjectConfiguration::setHomeScoresPageViewType(HomeScoresPageViewType type)
{
    // Intentionally not directly synced between instances
    // (it would be weird if you switch the view in one instance, and the others suddenly switch too)
    settings()->setLocalValue(HOME_SCORES_PAGE_VIEW_TYPE, Val(type));
}

QColor ProjectConfiguration::templatePreviewBackgroundColor() const
{
    return notationConfiguration()->backgroundColor();
}

kfw::async::Notification ProjectConfiguration::templatePreviewBackgroundChanged() const
{
    return notationConfiguration()->backgroundChanged();
}

ProjectConfiguration::PreferredScoreCreationMode ProjectConfiguration::preferredScoreCreationMode() const
{
    return settings()->value(PREFERRED_SCORE_CREATION_MODE_KEY).toEnum<PreferredScoreCreationMode>();
}

void ProjectConfiguration::setPreferredScoreCreationMode(PreferredScoreCreationMode mode)
{
    settings()->setSharedValue(PREFERRED_SCORE_CREATION_MODE_KEY, Val(mode));
}

static MigrationType migrationTypeFromString(const QString& str)
{
    if (str == "Pre_3_6") {
        return MigrationType::Pre_3_6;
    }
    if (str == "Ver_3_6") {
        return MigrationType::Ver_3_6;
    }
    LOGE() << "Unknown migration type string: " << str;
    return MigrationType::Unknown;
}

MigrationOptions ProjectConfiguration::migrationOptions(MigrationType type) const
{
    auto it = m_migrationOptions.find(type);
    if (it != m_migrationOptions.end()) {
        return it->second;
    }

    QString json = settings()->value(MIGRATION_OPTIONS).toQString();
    QJsonArray array = QJsonDocument::fromJson(json.toUtf8()).array();

    MigrationOptions result;

    for (const auto val : array) {
        QJsonObject obj = val.toObject();

        auto migrationType = migrationTypeFromString(obj["migrationType"].toString());
        if (migrationType == MigrationType::Unknown) {
            continue;
        }

        QJsonObject optionsObj = obj["options"].toObject();

        MigrationOptions opt;
        opt.appVersion = optionsObj["appVersion"].toInt(0);
        opt.isApplyMigration = optionsObj["isApplyMigration"].toBool();
        opt.isAskAgain = optionsObj["isAskAgain"].toBool();
        opt.isApplyLeland = optionsObj["isApplyLeland"].toBool();
        opt.isApplyEdwin = optionsObj["isApplyEdwin"].toBool();
        opt.isRemapPercussion = optionsObj["isRemapPercussion"].toBool();

        m_migrationOptions[migrationType] = opt;

        if (migrationType == type) {
            result = opt;
        }
    }

    return result;
}

static QString migrationTypeToString(MigrationType type)
{
    switch (type) {
    case MigrationType::Pre_3_6:
        return QStringLiteral("Pre_3_6");
    case MigrationType::Ver_3_6:
        return QStringLiteral("Ver_3_6");
    case MigrationType::Unknown:
        break;
    }
    return QString();
}

void ProjectConfiguration::setMigrationOptions(MigrationType type, const MigrationOptions& opt, bool persistent)
{
    m_migrationOptions[type] = opt;

    if (!persistent) {
        return;
    }

    QJsonArray array;

    for (auto it = m_migrationOptions.cbegin(); it != m_migrationOptions.cend(); ++it) {
        const MigrationOptions& o = it->second;

        QJsonObject options;
        options["appVersion"] = o.appVersion;
        options["isApplyMigration"] = o.isApplyMigration;
        options["isAskAgain"] = o.isAskAgain;
        options["isApplyLeland"] = o.isApplyLeland;
        options["isApplyEdwin"] = o.isApplyEdwin;
        options["isRemapPercussion"] = o.isRemapPercussion;

        QJsonObject obj;
        obj["migrationType"] = migrationTypeToString(it->first);
        obj["options"] = options;

        array << obj;
    }

    QString json = QJsonDocument(array).toJson(QJsonDocument::Compact);
    settings()->setSharedValue(MIGRATION_OPTIONS, Val(json));
}

bool ProjectConfiguration::isAutoSaveEnabled() const
{
    return settings()->value(AUTOSAVE_ENABLED_KEY).toBool();
}

void ProjectConfiguration::setAutoSaveEnabled(bool enabled)
{
    settings()->setSharedValue(AUTOSAVE_ENABLED_KEY, Val(enabled));
}

kfw::async::Channel<bool> ProjectConfiguration::autoSaveEnabledChanged() const
{
    return m_autoSaveEnabledChanged;
}

int ProjectConfiguration::autoSaveIntervalMinutes() const
{
    return settings()->value(AUTOSAVE_INTERVAL_KEY).toInt();
}

void ProjectConfiguration::setAutoSaveInterval(int minutes)
{
    settings()->setSharedValue(AUTOSAVE_INTERVAL_KEY, Val(minutes));
}

kfw::async::Channel<int> ProjectConfiguration::autoSaveIntervalChanged() const
{
    return m_autoSaveIntervalChanged;
}

kfw::io::path_t ProjectConfiguration::newProjectTemporaryPath() const
{
    return globalConfiguration()->userAppDataPath() + "/new_project" + DEFAULT_FILE_SUFFIX;
}

bool ProjectConfiguration::isAccessibleEnabled() const
{
    return accessibilityConfiguration()->enabled();
}

bool ProjectConfiguration::shouldDestinationFolderBeOpenedOnExport() const
{
    return settings()->value(SHOULD_DESTINATION_FOLDER_BE_OPENED_ON_EXPORT).toBool();
}

void ProjectConfiguration::setShouldDestinationFolderBeOpenedOnExport(bool shouldDestinationFolderBeOpenedOnExport)
{
    settings()->setSharedValue(SHOULD_DESTINATION_FOLDER_BE_OPENED_ON_EXPORT, Val(shouldDestinationFolderBeOpenedOnExport));
}

kfw::io::path_t ProjectConfiguration::projectBackupPath(const kfw::io::path_t& projectPath) const
{
    kfw::io::path_t projectDir = io::absoluteDirpath(projectPath);
    kfw::io::path_t projectName = io::filename(projectPath);

    return projectDir + "/.mscbackup/." + projectName + "~";
}

bool ProjectConfiguration::createBackupBeforeSaving() const
{
    return settings()->value(CREATE_BACKUP_BEFORE_SAVING).toBool();
}

void ProjectConfiguration::setCreateBackupBeforeSaving(bool create)
{
    settings()->setSharedValue(CREATE_BACKUP_BEFORE_SAVING, Val(create));
}

bool ProjectConfiguration::disableVersionChecking() const
{
    return settings()->value(DISABLE_VERSION_CHECKING).toBool();
}

void ProjectConfiguration::setDisableVersionChecking(bool disable)
{
    settings()->setSharedValue(DISABLE_VERSION_CHECKING, Val(disable));
}
