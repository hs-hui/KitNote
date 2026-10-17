/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include <QAbstractListModel>
#include <QTransform>
#include <qqmlintegration.h>

#include "async/asyncable.h"
#include "modularity/ioc.h"

#include "context/iglobalcontext.h"
#include "audio/main/iplayback.h"
#include "iplaybackcontroller.h"
#include "iplaybackconfiguration.h"
#include "notation/inotationconfiguration.h"

namespace kn::playback {
class NotationRegionsBeingProcessedModel : public QAbstractListModel, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(QVariant notationViewMatrix READ notationViewMatrix WRITE setNotationViewMatrix NOTIFY notationViewMatrixChanged)

    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY isEmptyChanged)

    Q_PROPERTY(QColor progressBackgroundColor READ progressBackgroundColor CONSTANT)
    Q_PROPERTY(QColor progressTextColor READ progressTextColor CONSTANT)

    QML_ELEMENT

    kfw::GlobalInject<IPlaybackConfiguration> configuration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::ContextInject<kn::context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::audio::IPlayback> playback = { this };
    kfw::ContextInject<IPlaybackController> playbackController = { this };

public:
    explicit NotationRegionsBeingProcessedModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void load();

    QVariant notationViewMatrix() const;

    bool isEmpty() const;

    QColor progressBackgroundColor() const;
    QColor progressTextColor() const;

public slots:
    void setNotationViewMatrix(const QVariant& matrix);

signals:
    void notationViewMatrixChanged();

    void isEmptyChanged();

private:
    enum Roles {
        RectRole = Qt::UserRole + 1,
        ProgressRole,
    };

    struct RegionInfo {
        kn::engraving::InstrumentTrackId trackId;
        QRectF logicRect;
        QRectF viewRect;

        bool operator==(const RegionInfo& r) const
        {
            return trackId == r.trackId && logicRect == r.logicRect && viewRect == r.viewRect;
        }
    };

    struct TickRange {
        int tickFrom = -1;
        int tickTo = -1;

        bool operator==(const TickRange& r) const
        {
            return tickFrom == r.tickFrom && tickTo == r.tickTo;
        }
    };

    struct TrackInfo {
        std::vector<TickRange> ranges;
        int progress = 0; // 0 - 100%

        bool operator==(const TrackInfo& t) const
        {
            return ranges == t.ranges && progress == t.progress;
        }
    };

    using TracksBeingProcessed = std::unordered_map<kn::engraving::InstrumentTrackId, TrackInfo>;
    using ChunkInfoList = kfw::audio::InputProcessingProgress::ChunkInfoList;

    bool isPlaying() const;

    void clear();
    void setRegions(const QList<RegionInfo>& regions);

    void onOnlineSoundsChanged();
    void onIsPlayingChanged();

    void listenViewModeChanges();

    void startListeningToProgress(const kfw::audio::TrackId trackId);
    void stopListeningToProgress(const kfw::audio::TrackId trackId);

    void onProgressStarted(const kn::engraving::InstrumentTrackId& instrumentTrackId);
    void onChunksReceived(const kn::engraving::InstrumentTrackId& instrumentTrackId, const ChunkInfoList& chunks);
    void onProgressChanged(const kn::engraving::InstrumentTrackId& instrumentTrackId, int progress);
    void onProgressFinished(const kn::engraving::InstrumentTrackId& instrumentTrackId);
    void onViewMatrixChanged();

    void initShouldShowRegions();

    QList<RegionInfo> calculateRegions(const TracksBeingProcessed& tracks) const;
    std::vector<QRectF> calculateRects(const kn::engraving::Part* part, const std::vector<TickRange>& ranges) const;
    std::vector<QRectF> calculateRects(const kn::engraving::Part* part, const kn::engraving::System* system,
                                       const std::vector<TickRange>& ranges) const;

    QTransform m_notationViewMatrix;
    std::map<kfw::audio::TrackId, kfw::audio::AudioResourceMeta> m_onlineSounds;
    TracksBeingProcessed m_tracksBeingProcessed;
    QList<RegionInfo> m_regions; // main model
    bool m_shouldShowRegions = true;
};
}
