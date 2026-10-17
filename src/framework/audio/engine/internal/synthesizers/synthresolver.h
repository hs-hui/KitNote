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

#ifndef KFW_AUDIO_SYNTHRESOLVER_H
#define KFW_AUDIO_SYNTHRESOLVER_H

#include <map>
#include <mutex>

#include "../../isynthresolver.h"

namespace kfw::audio::synth {
class SynthResolver : public ISynthResolver
{
public:
    void init(const AudioInputParams& defaultInputParams, const audio::OutputSpec& defaultOutputSpec) override;

    ISynthesizerPtr resolveSynth(const TrackId trackId, const AudioInputParams& params, const OutputSpec& spec,
                                 const PlaybackSetupData& setupData) const override;
    ISynthesizerPtr resolveDefaultSynth(const TrackId trackId) const override;
    AudioInputParams resolveDefaultInputParams() const override;
    AudioResourceMetaList resolveAvailableResources() const override;
    SoundPresetList resolveAvailableSoundPresets(const AudioResourceMeta& resourceMeta) const override;

    void registerResolver(const AudioSourceType type, IResolverPtr resolver) override;
    void unregisterResolver(const AudioSourceType type) override;

    void clearSources() override;

private:
    using SynthPair = std::pair<audio::AudioResourceId, ISynthesizerPtr>;

    mutable std::mutex m_mutex;

    std::map<AudioSourceType, IResolverPtr> m_resolvers;
    AudioInputParams m_defaultInputParams;
    OutputSpec m_defaultOutputSpec;
};
}

#endif // KFW_AUDIO_SYNTHRESOLVER_H
