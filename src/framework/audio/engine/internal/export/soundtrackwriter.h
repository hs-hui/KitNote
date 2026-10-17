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

#ifndef KFW_AUDIO_SOUNDTRACKWRITER_H
#define KFW_AUDIO_SOUNDTRACKWRITER_H

#include <vector>

#include "global/async/asyncable.h"

#include "global/modularity/ioc.h"
#include "../../iaudioengine.h"
#include "audio/common/rpc/irpcchannel.h"

#include "audio/common/audiotypes.h"
#include "../../iaudiosource.h"

#include "abstractaudioencoder.h"

namespace kfw::audio::soundtrack {
class SoundTrackWriter : public kfw::Contextable, public async::Asyncable
{
    kfw::ContextInject<engine::IAudioEngine> audioEngine = { this };
    kfw::ContextInject<rpc::IRpcChannel> rpcChannel = { this };

public:
    SoundTrackWriter(const io::path_t& destination, const SoundTrackFormat& format, const msecs_t totalDuration,
                     engine::IAudioSourcePtr source, const kfw::modularity::ContextPtr& iocCtx);
    ~SoundTrackWriter() override;

    Ret write();
    void abort();

    Progress progress();

private:
    Ret writeStreaming();

    void sendStreamingProgress(uint64_t framesWritten, uint64_t totalFrames);

    engine::IAudioSourcePtr m_source = nullptr;

    std::vector<float> m_intermBuffer;

    samples_t m_renderStep = 0;
    samples_t m_leadingSilenceSamples = 0;
    samples_t m_dataSamples = 0;
    samples_t m_totalSamples = 0;

    encode::AbstractAudioEncoderPtr m_encoderPtr = nullptr;

    Progress m_progress;
    std::atomic<bool> m_isAborted = false;
};
}

#endif // KFW_AUDIO_SOUNDTRACKWRITER_H
