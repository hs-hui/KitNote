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

#ifndef KFW_VST_VSTMODULESREPOSITORY_H
#define KFW_VST_VSTMODULESREPOSITORY_H

#include <unordered_map>
#include <mutex>

#include "ivstmodulesrepository.h"

#include "modularity/ioc.h"
#include "audioplugins/iknownaudiopluginsregister.h"
#include "audio/common/iaudiothreadsecurer.h"

#include "audioplugins/audiopluginstypes.h"
#include "vsttypes.h"

namespace kfw::vst {
class VstModulesRepository : public IVstModulesRepository, public kfw::Contextable
{
    kfw::GlobalInject<kfw::audio::IAudioThreadSecurer> threadSecurer;

    kfw::ContextInject<audioplugins::IKnownAudioPluginsRegister> knownPlugins { this };
public:
    VstModulesRepository(const kfw::modularity::ContextPtr& ctx)
        : kfw::Contextable(ctx) {}

    void init();
    void deInit();

    bool exists(const kfw::audio::AudioResourceId& resourceId) const override;
    PluginModulePtr pluginModule(const kfw::audio::AudioResourceId& resourceId) const override;
    void addPluginModule(const kfw::audio::AudioResourceId& resourceId) override;
    void removePluginModule(const kfw::audio::AudioResourceId& resourceId) override;

    audio::AudioResourceMetaList instrumentModulesMeta() const override;
    audio::AudioResourceMetaList fxModulesMeta() const override;
    void refresh() override;

private:
    audio::AudioResourceMetaList modulesMetaList(const audioplugins::AudioPluginType& type) const;

    PluginContext m_pluginContext;

    mutable std::mutex m_mutex;
    mutable std::unordered_map<audio::AudioResourceId, PluginModulePtr> m_modules;
};
}

#endif // KFW_VST_VSTMODULESREPOSITORY_H
