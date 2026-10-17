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

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QList>
#include <qqmlintegration.h>

#include "modularity/ioc.h"
#include "async/asyncable.h"
#include "audio/main/iplayback.h"
#include "context/iglobalcontext.h"
#include "playback/iplaybackconfiguration.h"
#include "ui/qml/KitNote/Ui/navigationsection.h"

#include "iplaybackcontroller.h"
#include "mixerchannelitem.h"

namespace kn::playback {
class MixerPanelModel : public QAbstractListModel, public QQmlParserStatus, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(
        kfw::ui::NavigationSection * navigationSection READ navigationSection WRITE setNavigationSection NOTIFY navigationSectionChanged)
    Q_PROPERTY(int navigationOrderStart READ navigationOrderStart WRITE setNavigationOrderStart NOTIFY navigationOrderStartChanged)

    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)

    QML_ELEMENT

    kfw::GlobalInject<IPlaybackConfiguration> configuration;
    kfw::ContextInject<kfw::audio::IPlayback> playback = { this };
    kfw::ContextInject<IPlaybackController> controller = { this };
    kfw::ContextInject<context::IGlobalContext> context = { this };

public:
    explicit MixerPanelModel(QObject* parent = nullptr);

    Q_INVOKABLE QVariantMap get(int index);
    Q_INVOKABLE void recheckRecording(); // KitNote

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    kfw::ui::NavigationSection* navigationSection() const;
    void setNavigationSection(kfw::ui::NavigationSection* navigationSection);

    int navigationOrderStart() const;
    void setNavigationOrderStart(int navigationOrderStart);

signals:
    void navigationSectionChanged();
    void navigationOrderStartChanged();
    void rowCountChanged();

private:
    void classBegin() override {}
    void componentComplete() override;
    void init();

    enum Roles {
        ChannelItemRole = Qt::UserRole + 1
    };

    void load();
    void loadItems();
    void onTrackAdded(const kfw::audio::TrackId& trackId);
    void addItem(MixerChannelItem* item, int index);
    void removeItem(const kfw::audio::TrackId trackId);
    void updateItemsPanelsOrder();
    void clear();
    void setupConnections();

    int resolveInsertIndex(const engraving::InstrumentTrackId& instrumentTrackId) const;
    int indexOf(const kfw::audio::TrackId trackId) const;

    MixerChannelItem* buildInstrumentChannelItem(const kfw::audio::TrackId trackId, const engraving::InstrumentTrackId& instrumentTrackId,
                                                 bool isPrimary = true);
    MixerChannelItem* buildAuxChannelItem(kfw::audio::aux_channel_idx_t index, const kfw::audio::TrackId trackId);
    MixerChannelItem* buildMasterChannelItem();

    int masterChannelIndex() const;

    void loadRecordingInfo(MixerChannelItem* item) const;
    void updateRecordingItem();

    MixerChannelItem* findChannelItem(const kfw::audio::TrackId& trackId) const;

    void loadOutputParams(MixerChannelItem* item, kfw::audio::AudioOutputParams&& params);
    void updateOutputResourceItemCount();

    project::INotationProjectPtr currentProject() const;
    project::IProjectAudioSettingsPtr audioSettings() const;
    notation::INotationPlaybackPtr notationPlayback() const;
    notation::INotationPartsPtr masterNotationParts() const;

    QList<MixerChannelItem*> m_mixerChannelList;
    MixerChannelItem* m_masterChannelItem = nullptr;
    kfw::audio::TrackSequenceId m_currentTrackSequenceId = -1;

    kfw::ui::NavigationSection* m_navigationSection = nullptr;
    int m_navigationOrderStart = 1;
};
}
