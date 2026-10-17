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

#ifndef KFW_VST_IVSTMODULESREPOSITORY_H
#define KFW_VST_IVSTMODULESREPOSITORY_H

#include "modularity/imoduleinterface.h"
#include "audio/common/audiotypes.h"

#include "vsttypes.h"

namespace kfw::vst {
class IVstModulesRepository : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IVstPluginRepository)

public:
    virtual ~IVstModulesRepository() = default;

    virtual bool exists(const kfw::audio::AudioResourceId& resourceId) const = 0;
    virtual PluginModulePtr pluginModule(const kfw::audio::AudioResourceId& resourceId) const = 0;
    virtual void addPluginModule(const kfw::audio::AudioResourceId& resourceId) = 0;
    virtual void removePluginModule(const kfw::audio::AudioResourceId& resourceId) = 0;
    virtual kfw::audio::AudioResourceMetaList instrumentModulesMeta() const = 0;
    virtual kfw::audio::AudioResourceMetaList fxModulesMeta() const = 0;
    virtual void refresh() = 0;
};
}

#endif // KFW_VST_IVSTMODULESREPOSITORY_H
