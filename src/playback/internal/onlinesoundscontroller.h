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

#include "actions/actionable.h"
#include "async/asyncable.h"

#include "modularity/ioc.h"

#include "playback/iplaybackconfiguration.h"
#include "actions/iactionsdispatcher.h"
#include "global/iinteractive.h"
#include "audio/main/iplayback.h"

namespace kn::playback {
class OnlineSoundsController : public kfw::actions::Actionable, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<IPlaybackConfiguration> configuration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<kfw::audio::IPlayback> playback = { this };

public:
    OnlineSoundsController(const kfw::modularity::ContextPtr& iocCtx);

    void regActions();

    void setCurrentSequence(kfw::audio::TrackSequenceId seqId);
    void resetCurrentSequence();

    void addOnlineTrack(const kfw::audio::TrackId trackId, const kfw::audio::AudioResourceMeta& meta);
    void removeOnlineTrack(const kfw::audio::TrackId trackId);

    bool shouldShowOnlineSoundsProcessingError(bool isPlaying) const;
    void showOnlineSoundsProcessingError(const std::function<void()>& onShown);

    const std::map<kfw::audio::TrackId, kfw::audio::AudioResourceMeta>& onlineSounds() const;
    kfw::async::Notification onlineSoundsChanged() const;
    kfw::Progress onlineSoundsProcessingProgress() const;

private:
    void listenProcessingProgress(const kfw::audio::TrackId trackId);
    void showLimitReachedErrorIfNeed(const kfw::audio::InputProcessingProgress::StatusInfo& status);

    void processOnlineSounds();

    kfw::audio::TrackSequenceId m_currentSequenceId = -1;
    std::map<kfw::audio::TrackId, kfw::audio::AudioResourceMeta> m_onlineSounds;
    std::unordered_set<kfw::audio::TrackId> m_onlineSoundsBeingProcessed;
    std::unordered_set<kfw::String> m_onlineLibrariesWithExceededLimit;
    kfw::async::Notification m_onlineSoundsChanged;
    kfw::Progress m_onlineSoundsProcessingProgress;
    kfw::Ret m_onlineSoundsProcessingRet;
};
}
