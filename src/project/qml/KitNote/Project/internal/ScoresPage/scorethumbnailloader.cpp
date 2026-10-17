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
#include "scorethumbnailloader.h"

using namespace kn::project;

ScoreThumbnailLoader::ScoreThumbnailLoader(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

QString ScoreThumbnailLoader::scorePath() const
{
    return m_scorePath;
}

void ScoreThumbnailLoader::setScorePath(const QString& scorePath)
{
    if (m_scorePath == scorePath) {
        return;
    }

    m_scorePath = scorePath;
    emit scorePathChanged();

    loadThumbnail();
}

bool ScoreThumbnailLoader::isThumbnailValid() const
{
    return !m_thumbnail.isNull();
}

QPixmap ScoreThumbnailLoader::thumbnail() const
{
    return m_thumbnail;
}

void ScoreThumbnailLoader::loadThumbnail()
{
    if (m_scorePath.isEmpty()) {
        setThumbnail(QPixmap());
        return;
    }

    recentFilesController()->thumbnail(m_scorePath)
    .onResolve(this, [this](const QPixmap& thumbnail) {
        setThumbnail(thumbnail);
    }).onReject(this, [this](int code, const std::string& error) {
        LOGE() << "Could not load thumbnail for " << m_scorePath << ": [" << code << "] " << error;
        setThumbnail(QPixmap());
    });
}

void ScoreThumbnailLoader::setThumbnail(const QPixmap& thumbnail)
{
    m_thumbnail = thumbnail;
    emit thumbnailChanged();
}
