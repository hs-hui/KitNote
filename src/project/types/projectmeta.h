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
#ifndef KN_PROJECT_PROJECTMETA_H
#define KN_PROJECT_PROJECTMETA_H

#include <QDate>
#include <QPixmap>
#include <QSet>
#include <QString>

#include "io/path.h"

namespace kn::project {
struct ProjectMeta
{
    kfw::io::path_t filePath;

    QString title;
    QString subtitle;
    QString composer;
    QString arranger;
    QString lyricist;
    QString translator;
    QString copyright;
    QDate creationDate;

    size_t partsCount = 0;
    QPixmap thumbnail;

    QString source;
    QString platform;
    QString kitnoteVersion;
    int kitnoteRevision = 0;
    int mscVersion = 0;

    QVariantMap additionalTags;

    kfw::io::path_t fileName(bool includingExtension = true) const
    {
        return kfw::io::filename(filePath, includingExtension);
    }

    bool operator==(const ProjectMeta& other) const
    {
        bool equal = filePath == other.filePath;
        equal &= title == other.title;
        equal &= subtitle == other.subtitle;
        equal &= composer == other.composer;
        equal &= arranger == other.arranger;
        equal &= lyricist == other.lyricist;
        equal &= translator == other.translator;
        equal &= copyright == other.copyright;
        equal &= creationDate == other.creationDate;

        equal &= partsCount == other.partsCount;
        equal &= thumbnail.toImage() == other.thumbnail.toImage();

        equal &= source == other.source;
        equal &= platform == other.platform;
        equal &= kitnoteVersion == other.kitnoteVersion;
        equal &= kitnoteRevision == other.kitnoteRevision;
        equal &= mscVersion == other.mscVersion;

        equal &= additionalTags == other.additionalTags;

        return equal;
    }

    bool operator!=(const ProjectMeta& other) const
    {
        return !(*this == other);
    }
};

using ProjectMetaList = QList<ProjectMeta>;

// Tags
inline const QString WORK_TITLE_TAG("workTitle");
inline const QString WORK_NUMBER_TAG("workNumber");
inline const QString SUBTITLE_TAG("subtitle");
inline const QString COMPOSER_TAG("composer");
inline const QString ARRANGER_TAG("arranger");
inline const QString LYRICIST_TAG("lyricist");
inline const QString COPYRIGHT_TAG("copyright");
inline const QString TRANSLATOR_TAG("translator");
inline const QString MOVEMENT_TITLE_TAG("movementTitle");
inline const QString MOVEMENT_NUMBER_TAG("movementNumber");
inline const QString CREATION_DATE_TAG("creationDate");
inline const QString PLATFORM_TAG("platform");

// Source tags
// TODO: don't use the score's meta tags to store this information
inline const QString SOURCE_TAG("source");
inline const QString SOURCE_REVISION_ID_TAG("sourceRevisionId");

inline bool isStandardTag(const QString& tag)
{
    static const QSet<QString> standardTags {
        WORK_TITLE_TAG,
        WORK_NUMBER_TAG,
        SUBTITLE_TAG,
        COMPOSER_TAG,
        ARRANGER_TAG,
        LYRICIST_TAG,
        COPYRIGHT_TAG,
        TRANSLATOR_TAG,
        MOVEMENT_TITLE_TAG,
        MOVEMENT_NUMBER_TAG,
        CREATION_DATE_TAG,
        PLATFORM_TAG,

        SOURCE_TAG,
        SOURCE_REVISION_ID_TAG
    };

    return standardTags.contains(tag);
}

inline bool isRepresentedInProjectMeta(const QString& tag)
{
    static const QSet<QString> projectMetaTags{
        WORK_TITLE_TAG,
        SUBTITLE_TAG,
        COMPOSER_TAG,
        ARRANGER_TAG,
        LYRICIST_TAG,
        COPYRIGHT_TAG,
        TRANSLATOR_TAG,
        CREATION_DATE_TAG,
        PLATFORM_TAG,

        SOURCE_TAG
    };

    return projectMetaTags.contains(tag);
}
}

#endif // KN_PROJECT_PROJECTMETA_H
