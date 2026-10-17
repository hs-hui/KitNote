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

#include <QTime>

#include "audio/common/audiotypes.h"

namespace kn::playback {
static constexpr kfw::audio::aux_channel_idx_t AUX_CHANNEL_NUM = 2;
static constexpr kfw::audio::aux_channel_idx_t REVERB_CHANNEL_IDX = 0;

enum class PlaybackCursorType {
    SMOOTH,
    STEPPED
};

enum class MixerSectionType {
    Unknown,
    Labels,
    Sound,
    AudioFX,
    Balance,
    Volume,
    Fader,
    MuteAndSolo,
    Title
};

inline QList<MixerSectionType> allMixerSectionTypes()
{
    static const QList<MixerSectionType> sections {
        MixerSectionType::Labels,
        MixerSectionType::Sound,
        MixerSectionType::AudioFX,
        MixerSectionType::Balance,
        MixerSectionType::Volume,
        MixerSectionType::Fader,
        MixerSectionType::MuteAndSolo,
        MixerSectionType::Title
    };

    return sections;
}

static const QTime ZERO_TIME(0, 0, 0, 0);

inline QTime timeFromSeconds(kfw::audio::secs_t seconds)
{
    return ZERO_TIME.addMSecs(kfw::audio::secsToMilisecs(seconds));
}

inline kfw::audio::secs_t timeToSeconds(const QTime& time)
{
    return kfw::audio::milisecsToSecs(ZERO_TIME.msecsTo(time));
}

enum class SoundProfileType {
    Undefined = -1,
    Basic,
    Muse,
    Custom
};

using SoundProfileName = kfw::String;
using SoundProfileData = std::map<kfw::mpe::PlaybackSetupData, kfw::audio::AudioResourceMeta>;

struct SoundProfile {
    SoundProfileType type = SoundProfileType::Undefined;
    SoundProfileName name;

    SoundProfileData data;

    const kfw::audio::AudioResourceMeta& findResource(const kfw::mpe::PlaybackSetupData& key) const
    {
        auto search = data.find(key);
        if (search != data.cend()) {
            return search->second;
        }

        static kfw::audio::AudioResourceMeta empty;
        return empty;
    }

    bool isEnabled() const
    {
        return !data.empty();
    }

    bool isValid() const
    {
        return type != SoundProfileType::Undefined
               && !name.isEmpty()
               && isEnabled();
    }
};

using SoundProfilesMap = std::map<SoundProfileName, SoundProfile>;

enum OnlineSoundsShowProgressBarMode {
    Always = 0,
    DuringPlayback,
    Never,
};
}
