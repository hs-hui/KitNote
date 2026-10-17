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

#include <qqmlintegration.h>

#include <QObject>

#include "modularity/ioc.h"
#include "async/asyncable.h"

#include "audio/main/iaudioconfiguration.h"
#include "audio/iaudiodrivercontroller.h"
#include "midi/imidiconfiguration.h"
#include "midi/imidioutport.h"
#include "midi/imidiinport.h"
#include "playback/iplaybackconfiguration.h"
#include "global/iinteractive.h"

namespace kn::preferences {
class AudioMidiPreferencesModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT
    QML_ELEMENT;

    Q_PROPERTY(int currentAudioApiIndex READ currentAudioApiIndex WRITE setCurrentAudioApiIndex NOTIFY currentAudioApiIndexChanged)

    Q_PROPERTY(QVariantList midiInputDevices READ midiInputDevices NOTIFY midiInputDevicesChanged)
    Q_PROPERTY(QString midiInputDeviceId READ midiInputDeviceId NOTIFY midiInputDeviceIdChanged)

    Q_PROPERTY(QVariantList midiOutputDevices READ midiOutputDevices NOTIFY midiOutputDevicesChanged)
    Q_PROPERTY(QString midiOutputDeviceId READ midiOutputDeviceId NOTIFY midiOutputDeviceIdChanged)

    Q_PROPERTY(bool isMIDI20OutputSupported READ isMIDI20OutputSupported CONSTANT)
    Q_PROPERTY(bool useMIDI20Output READ useMIDI20Output WRITE setUseMIDI20Output NOTIFY useMIDI20OutputChanged)

    Q_PROPERTY(bool muteHiddenInstruments READ muteHiddenInstruments WRITE setMuteHiddenInstruments NOTIFY muteHiddenInstrumentsChanged)

    Q_PROPERTY(
        bool shouldShowOnlineSoundsProcessingError READ shouldShowOnlineSoundsProcessingError WRITE setShouldShowOnlineSoundsProcessingError NOTIFY shouldShowOnlineSoundsProcessingErrorChanged)
    Q_PROPERTY(
        bool autoProcessOnlineSoundsInBackground READ autoProcessOnlineSoundsInBackground WRITE setAutoProcessOnlineSoundsInBackground NOTIFY autoProcessOnlineSoundsInBackgroundChanged)
    Q_PROPERTY(
        int onlineSoundsShowProgressBarMode READ onlineSoundsShowProgressBarMode WRITE setOnlineSoundsShowProgressBarMode NOTIFY onlineSoundsShowProgressBarModeChanged)
    Q_PROPERTY(
        bool useSoundFontLowPassFilter READ useSoundFontLowPassFilter WRITE setUseSoundFontLowPassFilter NOTIFY useSoundFontLowPassFilterChanged)

    kfw::GlobalInject<kfw::audio::IAudioConfiguration> audioConfiguration;
    kfw::GlobalInject<kfw::midi::IMidiConfiguration> midiConfiguration;
    kfw::GlobalInject<playback::IPlaybackConfiguration> playbackConfiguration;
    kfw::ContextInject<kfw::audio::IAudioDriverController> audioDriverController = { this };
    kfw::ContextInject<kfw::midi::IMidiOutPort> midiOutPort = { this };
    kfw::ContextInject<kfw::midi::IMidiInPort> midiInPort = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:
    explicit AudioMidiPreferencesModel(QObject* parent = nullptr);

    Q_INVOKABLE void init();

    int currentAudioApiIndex() const;

    QString midiInputDeviceId() const;
    Q_INVOKABLE void inputDeviceSelected(const QString& deviceId);

    QString midiOutputDeviceId() const;
    Q_INVOKABLE void outputDeviceSelected(const QString& deviceId);

    Q_INVOKABLE QStringList audioApiList() const;

    Q_INVOKABLE void restartAudioAndMidiDevices();

    Q_INVOKABLE bool onlineSoundsSectionVisible() const;

    QVariantList midiInputDevices() const;
    QVariantList midiOutputDevices() const;

    bool isMIDI20OutputSupported() const;
    bool useMIDI20Output() const;

    bool muteHiddenInstruments() const;

    bool shouldShowOnlineSoundsProcessingError() const;
    bool autoProcessOnlineSoundsInBackground() const;
    int onlineSoundsShowProgressBarMode() const;
    bool useSoundFontLowPassFilter() const;

public slots:
    void setCurrentAudioApiIndex(int index);

    void setUseMIDI20Output(bool use);

    void setMuteHiddenInstruments(bool mute);

    void setShouldShowOnlineSoundsProcessingError(bool value);
    void setAutoProcessOnlineSoundsInBackground(bool value);
    void setOnlineSoundsShowProgressBarMode(int mode);
    void setUseSoundFontLowPassFilter(bool value);

signals:
    void currentAudioApiIndexChanged(int index);
    void midiInputDeviceIdChanged();
    void midiOutputDeviceIdChanged();

    void midiInputDevicesChanged();
    void midiOutputDevicesChanged();

    void useMIDI20OutputChanged();

    void muteHiddenInstrumentsChanged(bool mute);

    void shouldShowOnlineSoundsProcessingErrorChanged();
    void autoProcessOnlineSoundsInBackgroundChanged();
    void onlineSoundsShowProgressBarModeChanged();
    void useSoundFontLowPassFilterChanged();

private:
    kfw::midi::MidiDeviceID midiInputDeviceId(int index) const;
    kfw::midi::MidiDeviceID midiOutputDeviceId(int index) const;

    void showMidiError(const kfw::midi::MidiDeviceID& deviceId, const std::string& text) const;
};
}
