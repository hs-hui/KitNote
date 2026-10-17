/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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

#ifndef KFW_VST_VSTPLUGINMETAREADER_H
#define KFW_VST_VSTPLUGINMETAREADER_H

#include "audioplugins/iaudiopluginmetareader.h"

namespace kfw::vst {
class VstPluginMetaReader : public audioplugins::IAudioPluginMetaReader
{
public:
    audio::AudioResourceType metaType() const override;
    bool canReadMeta(const io::path_t& pluginPath) const override;
    RetVal<kfw::audio::AudioResourceMetaList> readMeta(const io::path_t& pluginPath) const override;
};
}

#endif // KFW_VST_VSTPLUGINMETAREADER_H
