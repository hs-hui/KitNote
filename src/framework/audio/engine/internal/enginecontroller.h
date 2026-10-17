/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include <memory>

#include "global/modularity/ioc.h"

#include "../ienginecontroller.h"

namespace kfw::audio::rpc {
class IRpcChannel;
}

namespace kfw::audio::fx {
class FxResolver;
}

namespace kfw::audio::synth  {
class SynthResolver;
class SoundFontRepository;
}

namespace kfw::audio::engine {
class AudioEngineConfiguration;
class AudioEngine;
class WebAudioChannel;
class EnginePlayback;
class EngineRpcController;
class TransportEventsDispatcher;

class EngineController : public IEngineController, public kfw::Contextable
{
public:
    EngineController(std::shared_ptr<rpc::IRpcChannel> rpcChannel, const kfw::modularity::ContextPtr& iocCtx);

    void registerExports() override;
    void unregisterExports() override;

    void onStartRunning() override;
    void init(const OutputSpec& outputSpec, const AudioEngineConfig& conf) override;
    void deinit() override;

    OutputSpec outputSpec() const;
    async::Channel<OutputSpec> outputSpecChanged() const;

    void process(float* stream, unsigned samplesPerChannel);

    void process();
    void popAudioData(float* stream, unsigned samplesPerChannel);

private:
    std::shared_ptr<rpc::IRpcChannel> m_rpcChannel;
    std::shared_ptr<AudioEngineConfiguration> m_configuration;
    std::shared_ptr<AudioEngine> m_audioEngine;
    std::shared_ptr<EnginePlayback> m_playback;
    std::shared_ptr<EngineRpcController> m_rpcController;
    std::shared_ptr<fx::FxResolver> m_fxResolver;
    std::shared_ptr<synth::SynthResolver> m_synthResolver;
    std::shared_ptr<synth::SoundFontRepository> m_soundFontRepository;
    std::shared_ptr<WebAudioChannel> m_webAudioChannel;
    std::shared_ptr<TransportEventsDispatcher> m_transportEventsDispatcher;
};
}
