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

#ifndef KFW_AUDIO_FLUIDSYNTHCREATOR_H
#define KFW_AUDIO_FLUIDSYNTHCREATOR_H

#include <optional>
#include <unordered_map>

#include "../../../isynthresolver.h"

#include "global/async/asyncable.h"
#include "global/modularity/ioc.h"
#include "../../../isoundfontrepository.h"

#include "fluidsynth.h"

namespace kfw::audio::synth {
class FluidResolver : public ISynthResolver::IResolver, public kfw::Contextable, public async::Asyncable
{
    kfw::ContextInject<ISoundFontRepository> soundFontRepository = { this };

public:
    explicit FluidResolver(const kfw::modularity::ContextPtr& iocCtx = nullptr);
    ~FluidResolver() override;

    ISynthesizerPtr resolveSynth(const audio::TrackId trackId, const audio::AudioInputParams& params,
                                 const OutputSpec& spec) const override;
    bool hasCompatibleResources(const audio::PlaybackSetupData& setup) const override;

    audio::AudioResourceMetaList resolveResources() const override;
    audio::SoundPresetList resolveSoundPresets(const AudioResourceMeta& resourceMeta) const override;

    void refresh() override;
    void clearSources() override;

private:
    FluidSynthPtr createSynth(const audio::AudioResourceId& resourceId) const;

    struct SoundFontResource {
        io::path_t path;
        std::optional<midi::Program> preset = std::nullopt;
        AudioResourceMeta meta;
    };

    std::unordered_map<AudioResourceId, SoundFontResource> m_resourcesCache;
};
}

#endif // KFW_AUDIO_FLUIDSYNTHCREATOR_H
