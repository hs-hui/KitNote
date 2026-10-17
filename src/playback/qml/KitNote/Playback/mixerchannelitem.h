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

#include <QObject>
#include <qqmlintegration.h>

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "global/iinteractive.h"
#include "actions/iactionsdispatcher.h"
#include "context/iglobalcontext.h"
#include "iplaybackconfiguration.h"

#include "ui/qml/KitNote/Ui/navigationpanel.h"

#include "audio/common/audiotypes.h"
#include "inputresourceitem.h"
#include "outputresourceitem.h"
#include "auxsenditem.h"

namespace kn::playback {
class MixerChannelItem : public QObject, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(Type type READ type CONSTANT)
    Q_PROPERTY(bool outputOnly READ outputOnly CONSTANT)

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

    Q_PROPERTY(kn::playback::InputResourceItem * inputResourceItem READ inputResourceItem NOTIFY inputResourceItemChanged)
    Q_PROPERTY(
        QList<kn::playback::OutputResourceItem*> outputResourceItemList READ outputResourceItemList NOTIFY outputResourceItemListChanged)
    Q_PROPERTY(QList<kn::playback::AuxSendItem*> auxSendItemList READ auxSendItemList NOTIFY auxSendItemListChanged)

    Q_PROPERTY(float leftChannelPressure READ leftChannelPressure NOTIFY leftChannelPressureChanged)
    Q_PROPERTY(float rightChannelPressure READ rightChannelPressure NOTIFY rightChannelPressureChanged)

    Q_PROPERTY(float volumeLevel READ volumeLevel WRITE setVolumeLevel NOTIFY volumeLevelChanged)
    Q_PROPERTY(int balance READ balance WRITE setBalance NOTIFY balanceChanged)
    Q_PROPERTY(bool solo READ solo WRITE setSolo NOTIFY soloChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool forceMute READ forceMute NOTIFY forceMuteChanged)

    Q_PROPERTY(kfw::ui::NavigationPanel * panel READ panel NOTIFY panelChanged)

    // KitNote: the "Recording" channel - what its Sound slot says (the file, or why it is silent)
    Q_PROPERTY(QString recordingText READ recordingText NOTIFY recordingInfoChanged)
    Q_PROPERTY(QString recordingToolTip READ recordingToolTip NOTIFY recordingInfoChanged)
    Q_PROPERTY(bool recordingWarning READ recordingWarning NOTIFY recordingInfoChanged)

    QML_ELEMENT;
    QML_UNCREATABLE("Must be created in C++ only")

    kfw::GlobalInject<IPlaybackConfiguration> configuration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<context::IGlobalContext> context = { this };

public:
    enum class Type {
        Unknown,
        PrimaryInstrument,
        SecondaryInstrument,
        Metronome,
        Aux,
        Master,
        Recording,      // KitNote
    };
    Q_ENUM(Type)

    MixerChannelItem()
        : kfw::Contextable(kfw::iocCtxForQmlObject(this)) {}
    MixerChannelItem(QObject* parent, Type type, bool outputOnly = false, kfw::audio::TrackId trackId = -1);

    ~MixerChannelItem() override;

    Type type() const;

    kfw::audio::TrackId trackId() const;

    const engraving::InstrumentTrackId& instrumentTrackId() const;
    void setInstrumentTrackId(const engraving::InstrumentTrackId& instrumentTrackId);

    QString title() const;

    float leftChannelPressure() const;
    float rightChannelPressure() const;

    float volumeLevel() const;
    int balance() const;
    bool solo() const;
    bool muted() const;
    bool forceMute() const;

    kfw::ui::NavigationPanel* panel() const;
    void setPanelOrder(int panelOrder);
    void setPanelSection(kfw::ui::INavigationSection* section);

    void setOutputResourceItemCount(size_t count);

    void loadInputParams(const kfw::audio::AudioInputParams& newParams);
    void loadOutputParams(const kfw::audio::AudioOutputParams& newParams);
    void loadSoloMuteState(const notation::INotationSoloMuteState::SoloMuteState& newState);

    void subscribeOnAudioSignalChanges(kfw::audio::AudioSignalChanges&& audioSignalChanges);

    bool outputOnly() const;

    const kfw::audio::AudioInputParams& inputParams() const;
    const kfw::audio::AudioOutputParams& outputParams() const;

    InputResourceItem* inputResourceItem() const;
    QList<OutputResourceItem*> outputResourceItemList() const;
    QList<AuxSendItem*> auxSendItemList() const;

    const QMap<kfw::audio::aux_channel_idx_t, AuxSendItem*>& auxSendItems() const;

    QString recordingText() const;
    QString recordingToolTip() const;
    bool recordingWarning() const;
    void setRecordingInfo(const QString& text, const QString& toolTip, bool warning);
    Q_INVOKABLE void editRecording();

public slots:
    void setTitle(QString title);

    void setLeftChannelPressure(float leftChannelPressure);
    void setRightChannelPressure(float rightChannelPressure);

    void setVolumeLevel(float volumeLevel);
    void setBalance(int balance);
    void setSolo(bool solo);
    void setMuted(bool mute);

signals:
    void titleChanged(QString title);

    void leftChannelPressureChanged(float leftChannelPressure);
    void rightChannelPressureChanged(float rightChannelPressure);

    void volumeLevelChanged(float volumeLevel);
    void balanceChanged(int balance);
    void soloChanged();
    void mutedChanged();
    void forceMuteChanged();

    void panelChanged(kfw::ui::NavigationPanel* panel);

    void inputParamsChanged(const kfw::audio::AudioInputParams& params);
    void outputParamsChanged(const kfw::audio::AudioOutputParams& params);
    void soloMuteStateChanged(const notation::INotationSoloMuteState::SoloMuteState& state);

    void inputResourceItemChanged();
    void outputResourceItemListChanged();
    void auxSendItemListChanged();

    void recordingInfoChanged();

protected:
    notation::INotationPlaybackPtr notationPlayback() const;

    void setAudioChannelVolumePressure(const kfw::audio::audioch_t chNum, const float newValue);
    void resetAudioChannelsVolumePressure();

    void applyMuteToOutputParams(const bool isMuted);

    void loadOutputResourceItems(const kfw::audio::AudioFxChain& fxChain);
    void loadAuxSendItems(const kfw::audio::AuxSendsParams& auxSends);

    InputResourceItem* buildInputResourceItem();
    OutputResourceItem* buildOutputResourceItem(const kfw::audio::AudioFxParams& fxParams);
    AuxSendItem* buildAuxSendItem(kfw::audio::aux_channel_idx_t index, const kfw::audio::AuxSendParams& params);

    void addBlankSlots(size_t count);
    void removeBlankSlotsFromEnd(size_t count);

    kfw::audio::AudioFxChainOrder resolveNewBlankOutputResourceItemOrder() const;

    void openEditor(AbstractAudioResourceItem* item, const kfw::actions::ActionQuery& action);
    void closeEditor(AbstractAudioResourceItem* item);

    bool askAboutChangingSound();

    Type m_type = Type::Unknown;

    kfw::audio::TrackId m_trackId = -1;
    engraving::InstrumentTrackId m_instrumentTrackId;

    kfw::audio::AudioInputParams m_inputParams;
    kfw::audio::AudioOutputParams m_outParams;

    InputResourceItem* m_inputResourceItem = nullptr;
    QMap<kfw::audio::AudioFxChainOrder, OutputResourceItem*> m_outputResourceItems;
    QMap<kfw::audio::aux_channel_idx_t, AuxSendItem*> m_auxSendItems;

    kfw::audio::AudioSignalChanges m_audioSignalChanges;

    QString m_title;
    bool m_outputOnly = false;

    float m_leftChannelPressure = 0.0;
    float m_rightChannelPressure = 0.0;

    kfw::ui::NavigationPanel* m_panel = nullptr;

    bool m_outputResourceItemsLoading = false;

    QString m_recordingText;
    QString m_recordingToolTip;
    bool m_recordingWarning = false;
};
}
