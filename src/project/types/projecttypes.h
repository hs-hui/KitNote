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

#include <unordered_set>
#include <variant>

#include <QString>
#include <QUrl>

#include "containers.h"
#include "io/path.h"
#include "progress.h"
#include "log.h"

#include "projectmeta.h"

#include "notation/inotation.h"
#include "notation/notationtypes.h"
#include "../inotationwriter.h"

namespace kn::project {
struct ProjectCreateOptions
{
    QString title;
    QString subtitle;
    QString composer;
    QString lyricist;
    QString copyright;

    kfw::io::path_t templatePath;

    notation::ScoreCreateOptions scoreOptions;
};

struct OpenParams {
    OpenParams() {}

    kfw::io::path_t stylePath;
    bool disablePlayback = false;
    bool forceMode = false;
    bool forcePageMode = false;
    bool unrollRepeats = false;
};

struct MigrationOptions
{
    // common
    int appVersion = 0;
    bool isApplyMigration = false;
    bool isAskAgain = true;

    bool isApplyLeland = true;
    bool isApplyEdwin = true;
    bool isRemapPercussion = true;

    bool isValid() const { return appVersion != 0; }
};

enum class SaveMode
{
    Save,
    SaveAs,
    SaveCopy,
    SaveSelection,
    AutoSave,
    SavePage,
};

namespace _SaveLocationType {
Q_NAMESPACE;
enum class Type
{
    Undefined,
    Local
};
Q_ENUM_NS(Type)
}

using SaveLocationType = _SaveLocationType::Type;

struct ExportInfo {
    QString id;
    kfw::io::path_t exportPath;
    INotationWriter::UnitType unitType;
    std::vector<notation::INotationWeakPtr> notations;
};

struct SaveLocation
{
    SaveLocationType type = SaveLocationType::Undefined;
    kfw::io::path_t data;

    bool isLocal() const
    {
        return type == SaveLocationType::Local;
    }

    bool isValid() const
    {
        return isLocal();
    }

    const kfw::io::path_t& localPath() const
    {
        return data;
    }

    SaveLocation() = default;

    SaveLocation(SaveLocationType type, const kfw::io::path_t& data = {})
        : type(type), data(data) {}

    SaveLocation(const kfw::io::path_t& localPath)
        : type(SaveLocationType::Local), data(localPath) {}
};

struct ProjectFile {
    QUrl url;
    QString displayNameOverride = {};

    ProjectFile() = default;

    ProjectFile(const QUrl& url, const QString& displayNameOverride = {})
        : url(url), displayNameOverride(displayNameOverride) {}

    ProjectFile(const kfw::io::path_t& path, const QString& displayNameOverride = {})
        : url(path.toQUrl()), displayNameOverride(displayNameOverride) {}

    bool isNull() const
    {
        return url.isEmpty();
    }

    bool isValid() const
    {
        return url.isValid();
    }

    bool hasDisplayName() const
    {
        if (!displayNameOverride.isEmpty()) {
            return true;
        }

        return url.isLocalFile();
    }

    QString displayName(bool includingExtension) const
    {
        if (!displayNameOverride.isEmpty()) {
            return displayNameOverride;
        }

        return kfw::io::filename(path(), includingExtension).toQString();
    }

    kfw::io::path_t path() const
    {
        assert(url.isEmpty() || url.isLocalFile());

        return kfw::io::path_t(url);
    }

    bool operator ==(const ProjectFile& other) const
    {
        return url == other.url
               && displayNameOverride == other.displayNameOverride;
    }

    bool operator !=(const ProjectFile& other) const
    {
        return !(*this == other);
    }
};

using ProjectFilesList = std::vector<ProjectFile>;

struct RecentFile {
    kfw::io::path_t path;
    QString displayNameOverride = {};

    RecentFile() = default;

    RecentFile(const kfw::io::path_t& path, const QString& displayNameOverride = {})
        : path(path), displayNameOverride(displayNameOverride) {}

    static RecentFile fromProjectFile(const ProjectFile& projectFile)
    {
        return RecentFile(projectFile.path(), projectFile.displayNameOverride);
    }

    ProjectFile toProjectFile() const
    {
        return ProjectFile(path, displayNameOverride);
    }

    bool isValid() const
    {
        return !path.empty();
    }

    QString displayName(bool includingExtension) const
    {
        if (!displayNameOverride.isEmpty()) {
            return displayNameOverride;
        }

        return kfw::io::filename(path, includingExtension).toQString();
    }

    bool operator ==(const RecentFile& other) const
    {
        return path == other.path
               && displayNameOverride == other.displayNameOverride;
    }

    bool operator !=(const RecentFile& other) const
    {
        return !(*this == other);
    }
};

using RecentFilesList = std::vector<RecentFile>;

struct Template
{
    QString categoryTitle;
    ProjectMeta meta;
    bool isCustom = false;
};

using Templates = QList<Template>;

namespace _MigrationType {
Q_NAMESPACE;

enum class Type
{
    Unknown,
    Pre_3_6,
    Ver_3_6
};
Q_ENUM_NS(Type)
}

using MigrationType = _MigrationType::Type;

inline std::vector<MigrationType> allMigrationTypes()
{
    static const std::vector<MigrationType> types {
        MigrationType::Pre_3_6,
        MigrationType::Ver_3_6
    };

    return types;
}

inline bool isAudioFileSuffix(const std::string& suffix)
{
    static const std::unordered_set<std::string> audioSuffixes {
        "mp3", "wav", "ogg", "flac", "aac",
    };

    return kfw::contains(audioSuffixes, suffix);
}
}
