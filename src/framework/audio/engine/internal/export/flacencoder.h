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

#ifndef KFW_AUDIO_FLACENCODER_H
#define KFW_AUDIO_FLACENCODER_H

#include "abstractaudioencoder.h"

struct FlacHandler;

namespace kfw::audio::encode {
class FlacEncoder : public AbstractAudioEncoder
{
public:
    bool init(const io::path_t& path, const SoundTrackFormat& format, const samples_t totalSamplesNumber) override;

    size_t encode(samples_t samplesPerChannel, const float* input) override;
    size_t flush() override;

protected:
    size_t requiredOutputBufferSize(samples_t totalSamplesNumber) const override;
    void prepareOutputBuffer(const samples_t totalSamplesNumber) override;
    bool openDestination(const io::path_t& path) override;
    void closeDestination() override;

private:
    FlacHandler* m_flac = nullptr;
};
}

#endif // KFW_AUDIO_FLACENCODER_H
