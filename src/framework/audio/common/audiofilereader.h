/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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

#include <cstdint>
#include <vector>

namespace kfw::audio {
//! KitNote: a recording read whole into memory - its samples interleaved, as 16-bit integers (half the
//! memory of floats: a song of four minutes is about 40 MB).
struct AudioFileData {
    std::vector<int16_t> samples;
    unsigned int channels = 0;
    unsigned int sampleRate = 0;

    uint64_t frames() const
    {
        return channels > 0 ? samples.size() / channels : 0;
    }

    bool isValid() const
    {
        return channels > 0 && sampleRate > 0 && !samples.empty();
    }
};

//! KitNote: decodes a WAV, FLAC, MP3 or Ogg Vorbis file held in memory (dr_libs and stb_vorbis, both
//! already in src/framework/audio/thirdparty). Returns false for anything else (m4a, aac, aiff ...).
bool readAudioFile(const uint8_t* data, size_t size, AudioFileData& out);
}
