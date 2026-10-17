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

#include <QObject>
#include <qqmlintegration.h>

#include "async/asyncable.h"

#include "irecentfilescontroller.h"
#include "modularity/ioc.h"

namespace kn::project {
class ScoreThumbnailLoader : public QObject, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT;

    Q_PROPERTY(QString scorePath READ scorePath WRITE setScorePath NOTIFY scorePathChanged)

    Q_PROPERTY(bool isThumbnailValid READ isThumbnailValid NOTIFY thumbnailChanged)
    Q_PROPERTY(QPixmap thumbnail READ thumbnail NOTIFY thumbnailChanged)

    QML_ELEMENT

    kfw::ContextInject<IRecentFilesController> recentFilesController = { this };

public:
    ScoreThumbnailLoader(QObject* parent = nullptr);

    bool isThumbnailValid() const;
    QPixmap thumbnail() const;

    QString scorePath() const;
    void setScorePath(const QString& scorePath);

signals:
    void thumbnailChanged();

    void scorePathChanged();

private:
    void loadThumbnail();
    void setThumbnail(const QPixmap& thumbnail);

    QPixmap m_thumbnail;
    QString m_scorePath;
};
}
