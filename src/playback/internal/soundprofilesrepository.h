/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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

#ifndef KN_PLAYBACK_SOUNDPROFILESREPOSITORY_H
#define KN_PLAYBACK_SOUNDPROFILESREPOSITORY_H

#include <map>

#include "audio/main/iplayback.h"
#include "async/asyncable.h"
#include "modularity/ioc.h"

#include "isoundprofilesrepository.h"
#include "iplaybackconfiguration.h"

namespace kn::playback {
class SoundProfilesRepository : public ISoundProfilesRepository, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<IPlaybackConfiguration> config;
    kfw::ContextInject<kfw::audio::IPlayback> playback = { this };

public:
    SoundProfilesRepository(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    void init();

    void refresh() override;

    const SoundProfile& profile(const SoundProfileName& name) const override;
    bool containsProfile(const SoundProfileName& name) const override;
    const SoundProfilesMap& availableProfiles() const override;

    void addProfile(const SoundProfile& profile) override;
    void removeProfile(const SoundProfileName& name) override;

private:
    SoundProfilesMap m_profilesMap;
};
}

#endif // SOUNDPROFILESREPOSITORY_H
