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

#include "uicomponents/qml/KitNote/UiComponents/abstractmenumodel.h"

#include "modularity/ioc.h"
#include "iplaybackcontroller.h"
#include "notation/inotationconfiguration.h"
#include "context/iglobalcontext.h"

namespace kn::playback {
class PlaybackToolBarModel : public kfw::uicomponents::AbstractMenuModel
{
    Q_OBJECT

    Q_PROPERTY(bool isToolbarFloating READ isToolbarFloating WRITE setIsToolbarFloating NOTIFY isToolbarFloatingChanged)
    Q_PROPERTY(bool isPlayAllowed READ isPlayAllowed NOTIFY isPlayAllowedChanged)

    Q_PROPERTY(QTime maxPlayTime READ maxPlayTime NOTIFY maxPlayTimeChanged)

    Q_PROPERTY(QTime playTime READ playTime WRITE setPlayTime NOTIFY playPositionChanged)
    Q_PROPERTY(qreal playPosition READ playPosition WRITE setPlayPosition NOTIFY playPositionChanged)
    Q_PROPERTY(int measureNumber READ measureNumber WRITE setMeasureNumber NOTIFY playPositionChanged)
    Q_PROPERTY(int maxMeasureNumber READ maxMeasureNumber NOTIFY playPositionChanged)
    Q_PROPERTY(int beatNumber READ beatNumber WRITE setBeatNumber NOTIFY playPositionChanged)
    Q_PROPERTY(int maxBeatNumber READ maxBeatNumber NOTIFY playPositionChanged)

    Q_PROPERTY(QVariant tempo READ tempo NOTIFY tempoChanged)
    Q_PROPERTY(qreal tempoMultiplier READ tempoMultiplier WRITE setTempoMultiplier NOTIFY tempoChanged)
    // KitNote: the score plays a recording along with it - heard at 100% only
    Q_PROPERTY(bool hasRecording READ hasRecording NOTIFY hasRecordingChanged)

    QML_ELEMENT

    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::ContextInject<IPlaybackController> playbackController = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    explicit PlaybackToolBarModel(QObject* parent = nullptr);

    bool isToolbarFloating() const;
    bool isPlayAllowed() const;

    QTime maxPlayTime() const;
    QTime playTime() const;
    qreal playPosition() const;

    int measureNumber() const;
    int maxMeasureNumber() const;
    int beatNumber() const;
    int maxBeatNumber() const;

    QVariant tempo() const;
    qreal tempoMultiplier() const;
    bool hasRecording() const;

    Q_INVOKABLE void load() override;

public slots:
    void setIsToolbarFloating(bool floating);
    void setPlayPosition(qreal position);
    void setPlayTime(const QTime& time);
    void setMeasureNumber(int measureNumber);
    void setBeatNumber(int beatNumber);
    void setTempoMultiplier(qreal multiplier);

signals:
    void isToolbarFloatingChanged(bool floating);
    void isPlayAllowedChanged();
    void maxPlayTimeChanged();
    void playPositionChanged();
    void tempoChanged();
    void hasRecordingChanged();

private:
    void setupConnections();
    kfw::uicomponents::MenuItem* makeInputPitchMenu();

    void updateActions();
    void onActionsStateChanges(const kfw::actions::ActionCodeList& codes) override;

    bool isAdditionalAction(const kfw::actions::ActionCode& actionCode) const;

    kfw::secs_t totalPlayTime() const;
    notation::MeasureBeat measureBeat() const;

    kfw::ui::UiAction playAction() const;

    void updatePlayPosition(kfw::audio::secs_t secs);

    void rewind(kfw::audio::secs_t secs);
    void rewindToBeat(const notation::MeasureBeat& beat);

    bool m_isToolbarFloating = false;
    kfw::secs_t m_playbackPositionSecs = 0.0;
};
}
