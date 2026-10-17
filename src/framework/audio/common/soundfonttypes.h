/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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
#ifndef KFW_AUDIO_SOUNDFONTTYPES_H
#define KFW_AUDIO_SOUNDFONTTYPES_H

#include <map>
#include <string>
#include <vector>

#include "global/types/uri.h"
#include "global/io/path.h"

#include "midi/miditypes.h"

namespace kfw::audio::synth {
using SoundFontUri = Uri;
using SoundFontPath = io::path_t;

inline bool isSoundFont(const io::path_t& filePath)
{
    std::string ext = io::suffix(filePath);
    return ext == "sf2" || ext == "sf3";
}

struct SoundFontPreset
{
    midi::Program program;
    std::string name;
};

struct SoundFontMeta
{
    std::string name;
    SoundFontPath path;
    std::vector<SoundFontPreset> presets;
};

using SoundFontsMap = std::map<SoundFontUri, SoundFontMeta>;

inline const String SOUNDFONT_NAME_ATTRIBUTE(u"soundFontName");
inline const String PRESET_NAME_ATTRIBUTE(u"presetName");
inline const String PRESET_BANK_ATTRIBUTE(u"presetBank");
inline const String PRESET_PROGRAM_ATTRIBUTE(u"presetProgram");
}

#endif // KFW_AUDIO_SOUNDFONTTYPES_H
