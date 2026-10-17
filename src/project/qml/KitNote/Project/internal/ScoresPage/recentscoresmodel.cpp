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
#include "recentscoresmodel.h"

#include "translation.h"
#include "dataformatter.h"
#include "io/fileinfo.h"

#include "engraving/infrastructure/mscio.h"

#include "log.h"

using namespace kfw;
using namespace kn::project;

RecentScoresModel::RecentScoresModel(QObject* parent)
    : AbstractScoresModel(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void RecentScoresModel::load()
{
    updateRecentScores();

    recentFilesController()->recentFilesListChanged().onNotify(this, [this]() {
        updateRecentScores();
    });
}

void RecentScoresModel::setRecentScores(const std::vector<QVariantMap>& items)
{
    if (m_items == items) {
        return;
    }

    beginResetModel();
    m_items = items;
    endResetModel();
}

void RecentScoresModel::updateRecentScores()
{
    const RecentFilesList& recentScores = recentFilesController()->recentFilesList();

    std::vector<QVariantMap> items;
    items.reserve(recentScores.size());

    //! NOTE KitNote: no "New score" tile at the head of the list; the cards above the recent scores
    //! on the Home page start a new score.

    for (const RecentFile& file : recentScores) {
        QVariantMap obj;

        std::string suffix = io::suffix(file.path);
        bool isSuffixInteresting = !engraving::isCompressedScoreSuffix(suffix);

        RetVal<uint64_t> fileSize = fileSystem()->fileSize(file.path);
        QString fileSizeString = (fileSize.ret && fileSize.val > 0) ? DataFormatter::formatFileSize(fileSize.val).toQString() : QString();

        obj[NAME_KEY] = file.displayName(isSuffixInteresting);
        obj[PATH_KEY] = file.path.toQString();
        obj[SUFFIX_KEY] = QString::fromStdString(suffix);
        obj[FILE_SIZE_KEY] = fileSizeString;
        obj[TIME_SINCE_MODIFIED_KEY] = DataFormatter::formatTimeSince(io::FileInfo(file.path).lastModified().date()).toQString();
        obj[IS_CREATE_NEW_KEY] = false;
        obj[IS_NO_RESULTS_FOUND_KEY] = false;

        items.push_back(obj);
    }

    QVariantMap noResultsFoundItem;
    noResultsFoundItem[NAME_KEY] = "";
    noResultsFoundItem[IS_CREATE_NEW_KEY] = false;
    noResultsFoundItem[IS_NO_RESULTS_FOUND_KEY] = true;
    items.push_back(noResultsFoundItem);

    setRecentScores(items);
}

QList<int> RecentScoresModel::nonScoreItemIndices() const
{
    return { rowCount() - 1 };      // KitNote: the "no results" item; there is no "New score" item at the head
}
