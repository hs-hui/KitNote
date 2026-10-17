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

#ifndef KFW_AUDIO_OGGENCODER_H
#define KFW_AUDIO_OGGENCODER_H

#include "abstractaudioencoder.h"

struct OggOpusEnc;

namespace kfw::audio::encode {
class OggEncoder : public AbstractAudioEncoder
{
public:
    size_t encode(samples_t samplesPerChannel, const float* input) override;
    size_t flush() override;

protected:
    size_t requiredOutputBufferSize(samples_t) const override;
    bool openDestination(const io::path_t& path) override;
    void closeDestination() override;

private:
    OggOpusEnc* m_opusEncoder = nullptr;
};
}

#endif // OGGENCODER_H
