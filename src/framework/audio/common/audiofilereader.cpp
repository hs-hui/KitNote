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
#include "audiofilereader.h"

#include <cstdlib>
#include <cstring>

#if (defined (_MSCVER) || defined (_MSC_VER))
#pragma warning(push)
#pragma warning(disable: 4244 4456 4457 4245 4100 4706)
#endif

// The only place the decoders of dr_libs (public domain or MIT-0) are compiled.
#define DR_WAV_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#include "thirdparty/dr_libs/dr_wav.h"
#include "thirdparty/dr_libs/dr_flac.h"
#include "thirdparty/dr_libs/dr_mp3.h"

// stb_vorbis itself is compiled in engine/internal/codecs/vorbisdecoder.cpp: only its declarations here
#define STB_VORBIS_HEADER_ONLY
#include "thirdparty/stb/stb_vorbis.c"

#if (defined (_MSCVER) || defined (_MSC_VER))
#pragma warning(pop)
#endif

using namespace kfw::audio;

namespace {
bool startsWith(const uint8_t* data, size_t size, const char* magic)
{
    const size_t length = std::strlen(magic);
    return size >= length && std::memcmp(data, magic, length) == 0;
}

void take(int16_t* pcm, uint64_t frames, unsigned int channels, unsigned int sampleRate, AudioFileData& out)
{
    out.samples.assign(pcm, pcm + frames * channels);
    out.channels = channels;
    out.sampleRate = sampleRate;
}

bool readWav(const uint8_t* data, size_t size, AudioFileData& out)
{
    unsigned int channels = 0;
    unsigned int rate = 0;
    drwav_uint64 frames = 0;
    drwav_int16* pcm = drwav_open_memory_and_read_pcm_frames_s16(data, size, &channels, &rate, &frames, nullptr);
    if (!pcm) {
        return false;
    }
    take(pcm, frames, channels, rate, out);
    drwav_free(pcm, nullptr);
    return true;
}

bool readFlac(const uint8_t* data, size_t size, AudioFileData& out)
{
    unsigned int channels = 0;
    unsigned int rate = 0;
    drflac_uint64 frames = 0;
    drflac_int16* pcm = drflac_open_memory_and_read_pcm_frames_s16(data, size, &channels, &rate, &frames, nullptr);
    if (!pcm) {
        return false;
    }
    take(pcm, frames, channels, rate, out);
    drflac_free(pcm, nullptr);
    return true;
}

bool readMp3(const uint8_t* data, size_t size, AudioFileData& out)
{
    drmp3_config config {};
    drmp3_uint64 frames = 0;
    drmp3_int16* pcm = drmp3_open_memory_and_read_pcm_frames_s16(data, size, &config, &frames, nullptr);
    if (!pcm) {
        return false;
    }
    take(pcm, frames, config.channels, config.sampleRate, out);
    drmp3_free(pcm, nullptr);
    return true;
}

bool readVorbis(const uint8_t* data, size_t size, AudioFileData& out)
{
    int channels = 0;
    int rate = 0;
    short* pcm = nullptr;
    const int frames = stb_vorbis_decode_memory(data, static_cast<int>(size), &channels, &rate, &pcm);
    if (frames <= 0 || !pcm) {
        std::free(pcm);
        return false;
    }
    take(pcm, static_cast<uint64_t>(frames), static_cast<unsigned int>(channels), static_cast<unsigned int>(rate), out);
    std::free(pcm);
    return true;
}
}

bool kfw::audio::readAudioFile(const uint8_t* data, size_t size, AudioFileData& out)
{
    out = AudioFileData();
    if (!data || size < 4) {
        return false;
    }

    bool ok = false;
    if (startsWith(data, size, "RIFF") || startsWith(data, size, "RF64") || startsWith(data, size, "riff")) {
        ok = readWav(data, size, out);
    } else if (startsWith(data, size, "fLaC")) {
        ok = readFlac(data, size, out);
    } else if (startsWith(data, size, "OggS")) {
        ok = readVorbis(data, size, out);
    } else {
        // an MP3 has no magic of its own (an ID3 tag, or straight away a frame); a FLAC may begin with an ID3 tag too
        ok = readMp3(data, size, out) || readFlac(data, size, out);
    }

    return ok && out.isValid();
}
