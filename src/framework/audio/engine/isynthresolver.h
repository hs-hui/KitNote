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

#ifndef KFW_AUDIO_ISYNTHRESOLVER_H
#define KFW_AUDIO_ISYNTHRESOLVER_H

#include <memory>

#include "modularity/imoduleinterface.h"

#include "audio/common/audiotypes.h"

#include "isynthesizer.h"

namespace kfw::audio::synth {
class ISynthResolver : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(ISynthResolver)

public:
    virtual ~ISynthResolver() = default;

    class IResolver
    {
    public:
        virtual ~IResolver() = default;

        virtual ISynthesizerPtr resolveSynth(const TrackId trackId, const AudioInputParams& params, const OutputSpec& outputSpec) const = 0;
        virtual bool hasCompatibleResources(const PlaybackSetupData& setup) const = 0;
        virtual AudioResourceMetaList resolveResources() const = 0;
        virtual SoundPresetList resolveSoundPresets(const AudioResourceMeta& resourceMeta) const = 0;
        virtual void refresh() = 0;
        virtual void clearSources() = 0;
    };
    using IResolverPtr = std::shared_ptr<IResolver>;

    virtual void init(const AudioInputParams& defaultInputParams, const audio::OutputSpec& defaultOutputSpec) = 0;

    virtual ISynthesizerPtr resolveSynth(const TrackId trackId, const AudioInputParams& params, const OutputSpec& spec,
                                         const PlaybackSetupData& setupData) const = 0;
    virtual ISynthesizerPtr resolveDefaultSynth(const TrackId trackId) const = 0;
    virtual AudioInputParams resolveDefaultInputParams() const = 0;
    virtual AudioResourceMetaList resolveAvailableResources() const = 0;
    virtual SoundPresetList resolveAvailableSoundPresets(const AudioResourceMeta& resourceMeta) const = 0;
    virtual void registerResolver(const AudioSourceType type, IResolverPtr resolver) = 0;
    virtual void unregisterResolver(const AudioSourceType type) = 0;
    virtual void clearSources() = 0;
};

using ISynthResolverPtr = std::shared_ptr<ISynthResolver>;
}

#endif // KFW_AUDIO_ISYNTHRESOLVER_H
