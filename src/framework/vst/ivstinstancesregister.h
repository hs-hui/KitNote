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

#include "modularity/imoduleinterface.h"
#include "audio/common/audiotypes.h"

#include "vsttypes.h"
#include "ivstplugininstance.h"

namespace kfw::vst {
class IVstInstancesRegister : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IVstInstancesRegister)
public:
    virtual ~IVstInstancesRegister() = default;

    // make
    virtual IVstPluginInstancePtr makeAndRegisterInstrPlugin(const kfw::audio::AudioResourceId& resourceId,
                                                             const kfw::audio::TrackId trackId) = 0;

    virtual IVstPluginInstancePtr makeAndRegisterFxPlugin(const kfw::audio::AudioResourceId& resourceId,
                                                          const kfw::audio::TrackId trackId,
                                                          const kfw::audio::AudioFxChainOrder chainOrder) = 0;

    virtual IVstPluginInstancePtr makeAndRegisterMasterFxPlugin(const kfw::audio::AudioResourceId& resourceId,
                                                                const kfw::audio::AudioFxChainOrder chainOrder) = 0;
    // register
    virtual void registerInstrPlugin(const kfw::audio::TrackId trackId, IVstPluginInstancePtr instance) = 0;

    virtual void registerFxPlugin(const kfw::audio::TrackId trackId, const kfw::audio::AudioFxChainOrder chainOrder,
                                  IVstPluginInstancePtr instance) = 0;

    virtual void registerMasterFxPlugin(const kfw::audio::AudioFxChainOrder chainOrder, IVstPluginInstancePtr instance) = 0;

    // get
    virtual IVstPluginInstancePtr instanceById(const VstPluginInstanceId id) const = 0;

    virtual IVstPluginInstancePtr instrumentPlugin(const kfw::audio::AudioResourceId& resourceId,
                                                   const kfw::audio::TrackId trackId) const = 0;

    virtual IVstPluginInstancePtr fxPlugin(const kfw::audio::AudioResourceId& resourceId, const kfw::audio::TrackId trackId,
                                           const kfw::audio::AudioFxChainOrder chainOrder) const = 0;

    virtual IVstPluginInstancePtr masterFxPlugin(const kfw::audio::AudioResourceId& resourceId,
                                                 const kfw::audio::AudioFxChainOrder chainOrder) const = 0;

    // unregister
    virtual void unregisterById(const VstPluginInstanceId id) = 0;

    virtual void unregisterInstrPlugin(const kfw::audio::AudioResourceId& resourceId, const kfw::audio::TrackId trackId) = 0;

    virtual void unregisterFxPlugin(const kfw::audio::AudioResourceId& resourceId, const kfw::audio::TrackId trackId,
                                    const kfw::audio::AudioFxChainOrder chainOrder) = 0;

    virtual void unregisterMasterFxPlugin(const kfw::audio::AudioResourceId& resourceId,
                                          const kfw::audio::AudioFxChainOrder chainOrder) = 0;

    virtual void unregisterAllInstrPlugin() = 0;
    virtual void unregisterAllFx() = 0;
};
}
