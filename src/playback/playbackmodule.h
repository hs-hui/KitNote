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

#include "modularity/imodulesetup.h"

namespace kn::playback {
class PlaybackConfiguration;
class PlaybackController;
class PlaybackUiActions;
class SoundProfilesRepository;
class PlaybackModule : public kfw::modularity::IModuleSetup
{
public:

    std::string moduleName() const override;
    void registerExports() override;
    void resolveImports() override;
    void onInit(const kfw::IApplication::RunMode& mode) override;

private:
    std::shared_ptr<PlaybackConfiguration> m_configuration;
    std::shared_ptr<PlaybackController> m_playbackController;
    std::shared_ptr<PlaybackUiActions> m_playbackUiActions;
    std::shared_ptr<SoundProfilesRepository> m_soundProfileRepo;
};
}
