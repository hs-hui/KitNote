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

#include <memory>
#include <string>

#include "modularity/ioc.h"
#include "playback/iplaybackconfiguration.h"
#include "types/ret.h"
#include "engraving/infrastructure/mscreader.h"
#include "engraving/infrastructure/mscwriter.h"

#include "../iprojectaudiosettings.h"

namespace kn::project {
class ProjectAudioSettings : public IProjectAudioSettings, public kfw::Contextable
{
    kfw::GlobalInject<playback::IPlaybackConfiguration> playbackConfig;

public:
    const kfw::audio::AudioOutputParams& masterAudioOutputParams() const override;
    void setMasterAudioOutputParams(const kfw::audio::AudioOutputParams& params) override;

    bool containsAuxOutputParams(kfw::audio::aux_channel_idx_t index) const override;
    const kfw::audio::AudioOutputParams& auxOutputParams(kfw::audio::aux_channel_idx_t index) const override;
    void setAuxOutputParams(kfw::audio::aux_channel_idx_t index, const kfw::audio::AudioOutputParams& params) override;

    const TrackInputParamsMap& allTrackInputParams() const override;
    const kfw::audio::AudioInputParams& trackInputParams(const engraving::InstrumentTrackId& partId) const override;
    void setTrackInputParams(const engraving::InstrumentTrackId& partId, const kfw::audio::AudioInputParams& params) override;
    void clearTrackInputParams() override;
    kfw::async::Channel<engraving::InstrumentTrackId> trackInputParamsChanged() const override;

    bool trackHasExistingOutputParams(const engraving::InstrumentTrackId& partId) const override;
    const kfw::audio::AudioOutputParams& trackOutputParams(const engraving::InstrumentTrackId& partId) const override;
    void setTrackOutputParams(const engraving::InstrumentTrackId& partId, const kfw::audio::AudioOutputParams& params) override;

    const SoloMuteState& auxSoloMuteState(kfw::audio::aux_channel_idx_t index) const override;
    void setAuxSoloMuteState(kfw::audio::aux_channel_idx_t index, const SoloMuteState& state) override;
    kfw::async::Channel<kfw::audio::aux_channel_idx_t, SoloMuteState> auxSoloMuteStateChanged() const override;

    void removeTrackParams(const engraving::InstrumentTrackId& partId) override;

    const playback::SoundProfileName& activeSoundProfile() const override;
    void setActiveSoundProfile(const playback::SoundProfileName& profileName) override;

    kfw::async::Notification settingsChanged() const override;

    kfw::Ret read(const engraving::MscReader& reader);
    kfw::Ret write(engraving::MscWriter& writer, notation::INotationSoloMuteStatePtr masterSoloMuteStatePtr);

    //! NOTE Used for new or imported project (score)
    void makeDefault();

private:
    friend class NotationProject;
    ProjectAudioSettings(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    kfw::audio::AudioInputParams inputParamsFromJson(const QJsonObject& object) const;
    kfw::audio::AudioOutputParams outputParamsFromJson(const QJsonObject& object) const;
    SoloMuteState soloMuteStateFromJson(const QJsonObject& object) const;
    kfw::audio::AudioFxChain fxChainFromJson(const QJsonObject& fxChainObject) const;
    kfw::audio::AudioFxParams fxParamsFromJson(const QJsonObject& object) const;
    kfw::audio::AuxSendsParams auxSendsFromJson(const QJsonArray& objectList) const;
    kfw::audio::AuxSendParams auxSendParamsFromJson(const QJsonObject& object) const;
    kfw::audio::AudioResourceMeta resourceMetaFromJson(const QJsonObject& object) const;
    kfw::audio::AudioUnitConfig unitConfigFromJson(const QJsonObject& object) const;
    kfw::audio::AudioResourceAttributes attributesFromJson(const QJsonObject& object) const;

    QJsonObject inputParamsToJson(const kfw::audio::AudioInputParams& params) const;
    QJsonObject outputParamsToJson(const kfw::audio::AudioOutputParams& params) const;
    QJsonObject soloMuteStateToJson(const SoloMuteState& state) const;
    QJsonObject fxChainToJson(const kfw::audio::AudioFxChain& fxChain) const;
    QJsonObject fxParamsToJson(const kfw::audio::AudioFxParams& fxParams) const;
    QJsonArray auxSendsToJson(const kfw::audio::AuxSendsParams& auxSends) const;
    QJsonObject auxSendParamsToJson(const kfw::audio::AuxSendParams& auxParams) const;
    QJsonObject resourceMetaToJson(const kfw::audio::AudioResourceMeta& meta) const;
    QJsonObject unitConfigToJson(const kfw::audio::AudioUnitConfig& config) const;
    QJsonObject attributesToJson(const kfw::audio::AudioResourceAttributes& attributes) const;

    kfw::audio::AudioSourceType sourceTypeFromString(const QString& string) const;
    kfw::audio::AudioResourceType resourceTypeFromString(const QString& string) const;

    QString sourceTypeToString(const kfw::audio::AudioSourceType& type) const;

    QJsonObject buildAuxObject(kfw::audio::aux_channel_idx_t index, const kfw::audio::AudioOutputParams& params) const;
    QJsonObject buildTrackObject(notation::INotationSoloMuteStatePtr masterSoloMuteStatePtr, const engraving::InstrumentTrackId& id) const;

    kfw::audio::AudioOutputParams m_masterOutputParams;

    std::map<kfw::audio::aux_channel_idx_t, kfw::audio::AudioOutputParams> m_auxOutputParams;
    std::unordered_map<kfw::audio::aux_channel_idx_t, SoloMuteState> m_auxSoloMuteStatesMap;
    kfw::async::Channel<kfw::audio::aux_channel_idx_t, SoloMuteState> m_auxSoloMuteStateChanged;

    std::unordered_map<engraving::InstrumentTrackId, kfw::audio::AudioInputParams> m_trackInputParamsMap;
    std::unordered_map<engraving::InstrumentTrackId, kfw::audio::AudioOutputParams> m_trackOutputParamsMap;

    kfw::async::Notification m_settingsChanged;
    kfw::async::Channel<engraving::InstrumentTrackId> m_trackInputParamsChanged;

    kn::playback::SoundProfileName m_activeSoundProfileName;
};

using ProjectAudioSettingsPtr = std::shared_ptr<ProjectAudioSettings>;
}
