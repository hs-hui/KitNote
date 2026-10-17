/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include "global/async/asyncable.h"

#include "../iplaybackstate.h"
#include "audio/main/iplayer.h"

namespace kn::context {
class PlaybackState : public IPlaybackState, public kfw::async::Asyncable
{
public:
    PlaybackState() = default;

    void setPlayer(kfw::audio::IPlayerPtr player);

    kfw::audio::PlaybackStatus playbackStatus() const override;
    kfw::async::Channel<kfw::audio::PlaybackStatus> playbackStatusChanged() const override;

    kfw::audio::secs_t playbackPosition() const override;
    kfw::async::Channel<kfw::audio::secs_t> playbackPositionChanged() const override;

private:
    kfw::audio::IPlayerPtr m_player;

    kfw::async::Channel<kfw::audio::PlaybackStatus> m_playbackStatusChanged;
    kfw::async::Channel<kfw::audio::secs_t> m_playbackPositionChanged;
};
}
