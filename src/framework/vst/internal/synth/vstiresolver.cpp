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

#include "vstiresolver.h"

#include "log.h"

using namespace kfw::vst;
using namespace kfw::audio;
using namespace kfw::audio::synth;

ISynthesizerPtr VstiResolver::resolveSynth(const audio::TrackId trackId, const audio::AudioInputParams& params,
                                           const audio::OutputSpec& outputSpec) const
{
    return createSynth(trackId, params, outputSpec);
}

bool VstiResolver::hasCompatibleResources(const kfw::audio::PlaybackSetupData& /*setup*/) const
{
    return true;
}

void VstiResolver::refresh()
{
    pluginModulesRepo()->refresh();
}

VstSynthPtr VstiResolver::createSynth(const TrackId trackId, const AudioInputParams& params, const OutputSpec& outputSpec) const
{
    if (!pluginModulesRepo()->exists(params.resourceMeta.id)) {
        return nullptr;
    }

    auto synth = std::make_shared<VstSynthesiser>(trackId, params, iocContext());
    synth->init(outputSpec);

    return synth;
}

AudioResourceMetaList VstiResolver::resolveResources() const
{
    return pluginModulesRepo()->instrumentModulesMeta();
}

SoundPresetList VstiResolver::resolveSoundPresets(const kfw::audio::AudioResourceMeta&) const
{
    return SoundPresetList();
}

void VstiResolver::clearSources()
{
    instancesRegister()->unregisterAllInstrPlugin();
}
