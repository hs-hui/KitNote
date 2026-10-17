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

#include <mutex>
#include <atomic>

#include "../ivstplugininstance.h"

#include "modularity/ioc.h"
#include "async/asyncable.h"
#include "async/notification.h"
#include "async/channel.h"
#include "audio/common/iaudiothreadsecurer.h"
#include "audio/common/audiotypes.h"

#include "../ivstmodulesrepository.h"
#include "../vsttypes.h"
#include "vstcomponenthandler.h"

namespace kfw::vst {
class VstPluginProvider;
class VstPluginInstance : public IVstPluginInstance, public async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<kfw::audio::IAudioThreadSecurer> threadSecurer;

    kfw::ContextInject<IVstModulesRepository> modulesRepo = { this };

public:
    VstPluginInstance(const kfw::audio::AudioResourceId& resourceId, const modularity::ContextPtr& iocCtx);
    ~VstPluginInstance() override;

    const kfw::audio::AudioResourceId& resourceId() const override;
    const std::string& name() const override;
    VstPluginInstanceId id() const override;

    PluginViewPtr createView() const override;

    PluginControllerPtr controller() const override;
    PluginComponentPtr component() const override;
    PluginMidiMappingPtr midiMapping() const override;

    void updatePluginConfig(const kfw::audio::AudioUnitConfig& config) override;
    void refreshConfig() override;

    void load();

    bool isValid() const;
    bool isLoaded() const override;

    async::Notification loadingCompleted() const override;

    async::Channel<kfw::audio::AudioUnitConfig> pluginSettingsChanged() const override;

private:
    void syncControllerToComponentState();
    void rescanParams();
    void setPluginConfig(const kfw::audio::AudioUnitConfig& config);

    VstPluginInstanceId m_id = 0;
    kfw::audio::AudioResourceId m_resourceId;

    PluginModulePtr m_module = nullptr;
    std::unique_ptr<VstPluginProvider> m_pluginProvider;

    Steinberg::FUnknownPtr<VstComponentHandler> m_componentHandlerPtr = nullptr;

    VstMemoryStream m_componentStateBuffer;
    VstMemoryStream m_controllerStateBuffer;
    mutable async::Channel<kfw::audio::AudioUnitConfig> m_pluginSettingsChanges;

    std::atomic_bool m_isLoaded = false;
    async::Notification m_loadingCompleted;

    std::thread::id m_mainThreadId;

    mutable std::mutex m_mutex;
};
}
