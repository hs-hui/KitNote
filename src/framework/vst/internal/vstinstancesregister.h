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

#include <map>
#include <mutex>

#include "global/modularity/ioc.h"

#include "../ivstinstancesregister.h"
#include "async/asyncable.h"

namespace kfw::vst {
class VstInstancesRegister : public IVstInstancesRegister, public async::Asyncable, public kfw::Contextable
{
public:

    VstInstancesRegister(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    // make
    IVstPluginInstancePtr makeAndRegisterInstrPlugin(const kfw::audio::AudioResourceId& resourceId,
                                                     const kfw::audio::TrackId trackId) override;

    IVstPluginInstancePtr makeAndRegisterFxPlugin(const kfw::audio::AudioResourceId& resourceId, const kfw::audio::TrackId trackId,
                                                  const kfw::audio::AudioFxChainOrder chainOrder) override;

    IVstPluginInstancePtr makeAndRegisterMasterFxPlugin(const kfw::audio::AudioResourceId& resourceId,
                                                        const kfw::audio::AudioFxChainOrder chainOrder) override;

    // register
    void registerInstrPlugin(const kfw::audio::TrackId trackId, IVstPluginInstancePtr instance) override;

    void registerFxPlugin(const kfw::audio::TrackId trackId, const kfw::audio::AudioFxChainOrder chainOrder,
                          IVstPluginInstancePtr instance) override;

    void registerMasterFxPlugin(const kfw::audio::AudioFxChainOrder chainOrder, IVstPluginInstancePtr instance) override;

    // get
    IVstPluginInstancePtr instanceById(const VstPluginInstanceId id) const override;
    IVstPluginInstancePtr instrumentPlugin(const kfw::audio::AudioResourceId& resourceId,
                                           const kfw::audio::TrackId trackId) const override;
    IVstPluginInstancePtr fxPlugin(const kfw::audio::AudioResourceId& resourceId, const kfw::audio::TrackId trackId,
                                   const kfw::audio::AudioFxChainOrder chainOrder) const override;
    IVstPluginInstancePtr masterFxPlugin(const kfw::audio::AudioResourceId& resourceId,
                                         const kfw::audio::AudioFxChainOrder chainOrder) const override;

    // unregister
    void unregisterById(const VstPluginInstanceId id) override;
    void unregisterInstrPlugin(const kfw::audio::AudioResourceId& resourceId, const kfw::audio::TrackId trackId) override;
    void unregisterFxPlugin(const kfw::audio::AudioResourceId& resourceId, const kfw::audio::TrackId trackId,
                            const kfw::audio::AudioFxChainOrder chainOrder) override;
    void unregisterMasterFxPlugin(const kfw::audio::AudioResourceId& resourceId, const kfw::audio::AudioFxChainOrder chainOrder) override;

    void unregisterAllInstrPlugin() override;
    void unregisterAllFx() override;

private:
    mutable std::mutex m_mutex;

    enum class Type {
        Undefined = 0,
        Instrument,
        Effect
    };

    struct Key {
        Type type = Type::Undefined;
        kfw::audio::AudioResourceId resourceId;
        kfw::audio::TrackId trackId = -1;
        kfw::audio::AudioFxChainOrder chainOrder = 0;

        inline bool operator ==(const Key& k) const
        {
            return type == k.type && trackId == k.trackId && resourceId == k.resourceId && chainOrder == k.chainOrder;
        }

        inline bool operator <(const Key& k) const
        {
            if (type != k.type) {
                return type < k.type;
            }

            if (trackId != k.trackId) {
                return trackId < k.trackId;
            }

            if (resourceId != k.resourceId) {
                return resourceId < k.resourceId;
            }

            return chainOrder < k.chainOrder;
        }
    };

    std::map<Key, IVstPluginInstancePtr> m_instances;
};
}
