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

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "playback/iplaybackcontroller.h"
#include "audio/main/iaudioconfiguration.h"
#include "actions/iactionsdispatcher.h"
#include "context/iglobalcontext.h"
#include "tours/itoursservice.h"

namespace kn::playback {
class OnlineSoundsStatusModel : public QObject, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(bool hasOnlineSounds READ hasOnlineSounds NOTIFY hasOnlineSoundsChanged)
    Q_PROPERTY(bool manualProcessingAllowed READ manualProcessingAllowed NOTIFY manualProcessingAllowedChanged)

    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorTitle READ errorTitle NOTIFY statusChanged)
    Q_PROPERTY(QString errorDescription READ errorDescription NOTIFY statusChanged)

    QML_ELEMENT

    kfw::GlobalInject<kfw::audio::IAudioConfiguration> audioConfiguration;
    kfw::ContextInject<IPlaybackController> playbackController = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<kfw::tours::IToursService> tours = { this };

public:
    enum class Status {
        Processing,
        Success,
        Error,
    };

    Q_ENUM(Status);

    explicit OnlineSoundsStatusModel(QObject* parent = nullptr);

    Q_INVOKABLE void load();
    Q_INVOKABLE void processOnlineSounds();

    bool hasOnlineSounds() const;
    bool manualProcessingAllowed() const;

    int status() const;
    QString errorTitle() const;
    QString errorDescription() const;

signals:
    void hasOnlineSoundsChanged();
    void manualProcessingAllowedChanged();
    void statusChanged();

private:
    void onOnlineSoundsChanged();
    void updateManualProcessingAllowed(bool enableByDefault = true);

    void setManualProcessingAllowed(bool allowed);
    void setStatus(Status status);

    notation::InstrumentTrackIdSet m_onlineTrackIdSet;
    bool m_manualProcessingAllowed = false;
    bool m_shouldNotifyToursThatManualProcessingAllowed = true;
    Status m_status = Status::Success;
    kfw::Ret m_ret;
    kfw::async::Channel<notation::InstrumentTrackIdSet> m_tracksDataChanged;
};
}
