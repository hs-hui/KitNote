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

#include "async/notification.h"
#include "audio/common/audiotypes.h"
#include "engraving/types/types.h"
#include "playback/playbacktypes.h"
#include "notation/inotationsolomutestate.h"

namespace kn::project {
using TrackInputParamsMap = std::unordered_map<engraving::InstrumentTrackId, kfw::audio::AudioInputParams>;

class IProjectAudioSettings
{
public:
    using SoloMuteState = notation::INotationSoloMuteState::SoloMuteState;

    virtual ~IProjectAudioSettings() = default;

    virtual const kfw::audio::AudioOutputParams& masterAudioOutputParams() const = 0;
    virtual void setMasterAudioOutputParams(const kfw::audio::AudioOutputParams& params) = 0;

    virtual bool containsAuxOutputParams(kfw::audio::aux_channel_idx_t index) const = 0;
    virtual const kfw::audio::AudioOutputParams& auxOutputParams(kfw::audio::aux_channel_idx_t index) const = 0;
    virtual void setAuxOutputParams(kfw::audio::aux_channel_idx_t index, const kfw::audio::AudioOutputParams& params) = 0;

    virtual const TrackInputParamsMap& allTrackInputParams() const = 0;
    virtual const kfw::audio::AudioInputParams& trackInputParams(const engraving::InstrumentTrackId& trackId) const = 0;
    virtual void setTrackInputParams(const engraving::InstrumentTrackId& trackId, const kfw::audio::AudioInputParams& params) = 0;
    virtual void clearTrackInputParams() = 0;
    virtual kfw::async::Channel<engraving::InstrumentTrackId> trackInputParamsChanged() const = 0;

    virtual bool trackHasExistingOutputParams(const engraving::InstrumentTrackId& trackId) const = 0;
    virtual const kfw::audio::AudioOutputParams& trackOutputParams(const engraving::InstrumentTrackId& trackId) const = 0;
    virtual void setTrackOutputParams(const engraving::InstrumentTrackId& trackId, const kfw::audio::AudioOutputParams& params) = 0;

    virtual const SoloMuteState& auxSoloMuteState(kfw::audio::aux_channel_idx_t index) const = 0;
    virtual void setAuxSoloMuteState(kfw::audio::aux_channel_idx_t index, const SoloMuteState& state) = 0;
    virtual kfw::async::Channel<kfw::audio::aux_channel_idx_t, SoloMuteState> auxSoloMuteStateChanged() const = 0;

    virtual void removeTrackParams(const engraving::InstrumentTrackId& trackId) = 0;

    virtual const playback::SoundProfileName& activeSoundProfile() const = 0;
    virtual void setActiveSoundProfile(const playback::SoundProfileName& profileName) = 0;

    virtual kfw::async::Notification settingsChanged() const = 0;
};

using IProjectAudioSettingsPtr = std::shared_ptr<IProjectAudioSettings>;
}
