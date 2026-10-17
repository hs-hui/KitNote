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
#include "webaudioengine.h"

#include "global/modularity/ioc.h"

#include "global/runtime.h"

#include "audio/common/rpc/platform/web/webrpcchannel.h"

#include "audio/engine/internal/enginecontroller.h"

#include "log.h"

using namespace kfw;
using namespace kfw::io;
using namespace kfw::audio;
using namespace kfw::audio::engine;
using namespace kfw::audio::rpc;

WebAudioEngine* WebAudioEngine::instance()
{
    static WebAudioEngine w;
    return &w;
}

static std::string moduleName()
{
    return "audio_engine";
}

static modularity::ModulesIoC* ioc()
{
    return modularity::globalIoc();
}

void WebAudioEngine::init()
{
    kfw::runtime::mainThreadId(); //! NOTE Needs only call
    kfw::runtime::setThreadName("audio_engine");

    //! --- Setup logger ---
    using namespace kfw::logger;
    Logger* logger = Logger::instance();
    logger->clearDests();

    //! Console
    if (kfw::runtime::isDebug()) {
        class ThreadNameProvider : public IThreadNameProvider
        {
        public:
            const std::string& threadName(const std::thread::id&) const { return kfw::runtime::threadName(); }
        };

        LogLayout ll("${time} | ${type|5} | ${thread|15} | ${tag|15} | ${message}");
        ll.setThreadNameProvider(std::make_shared<ThreadNameProvider>());
        logger->addDest(new ConsoleLogDest(ll));
    }

    rpc::set_last_stream_id(100000);
    m_rpcChannel = std::make_shared<WebRpcChannel>();
    m_rpcChannel->setupOnEngine();
    ioc()->registerExport<IRpcChannel>(moduleName(), m_rpcChannel);

    m_controller = std::make_shared<EngineController>(m_rpcChannel);
    m_controller->registerExports();
    m_controller->onStartRunning();

    LOGI() << "Web audio engine running";
}

void WebAudioEngine::process(float* stream, unsigned samplesPerChannel)
{
    m_controller->process(stream, samplesPerChannel);
}
