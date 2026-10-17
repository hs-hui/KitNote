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
#ifndef KFW_AUDIO_AUDIOENGINE_H
#define KFW_AUDIO_AUDIOENGINE_H

#include <memory>
#include <atomic>
#include <mutex>

#include "../iaudioengine.h"

#include "global/types/ret.h"

namespace kfw::audio::engine {
class AudioBuffer;
class AudioEngine : public IAudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();

    struct RenderConstraints {
        samples_t minSamplesToReserveWhenIdle = 0;
        samples_t minSamplesToReserveInRealtime = 0;

        // mixer
        size_t desiredAudioThreadNumber = 0;
        size_t minTrackCountForMultithreading = 0;
    };

    Ret init(const OutputSpec& outputSpec, const RenderConstraints& consts);
    void deinit();

    void setOutputSpec(const OutputSpec& outputSpec) override;
    OutputSpec outputSpec() const override;
    async::Channel<OutputSpec> outputSpecChanged() const override;

    RenderMode mode() const override;
    void setMode(const RenderMode newMode) override;
    async::Channel<RenderMode> modeChanged() const override;

    void execOperation(OperationType type, const Operation& func) override;
    OperationType operation() const override;

    MixerPtr mixer() const override;

    void processAudioData() override;
    samples_t process(float* buffer, samples_t samplesPerChannel) override;
    void popAudioData(float* dest, size_t sampleCount) override;

private:

    void updateBufferConstraints();
    samples_t fillSilent(float* buffer, samples_t samplesPerChannel);

    std::atomic<bool> m_inited = false;

    OutputSpec m_outputSpec;
    async::Channel<OutputSpec> m_outputSpecChanged;

    std::atomic<RenderMode> m_mode = RenderMode::Undefined;
    async::Channel<RenderMode> m_modeChanged;

    std::atomic<bool> m_processing = false;
    std::atomic<OperationType> m_operationType = OperationType::Undefined;
    std::mutex m_quickOperationWaitMutex;

    MixerPtr m_mixer = nullptr;
    std::shared_ptr<AudioBuffer> m_buffer = nullptr;
    RenderConstraints m_renderConsts;
};
}

#endif // KFW_AUDIO_AUDIOENGINE_H
