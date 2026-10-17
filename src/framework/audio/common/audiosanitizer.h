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
#ifndef KFW_AUDIO_AUDIOSANITIZER_H
#define KFW_AUDIO_AUDIOSANITIZER_H

//! NOTE This is dev tools

#include <cassert>
#include <set>
#include <thread>

#include "kfw_framework_config.h"

namespace kfw::audio {
class AudioSanitizer
{
public:

    static void setupMainThread();
    static std::thread::id mainThread();
    static bool isMainThread();

    static void setupEngineThread();
    static void setMixerThreads(const std::set<std::thread::id>& threadIdSet);
    static std::thread::id engineThread();
    static bool isEngineThread();
};
}

#define AUDIO_SANITIZER_ENABLED

#ifdef Q_OS_WASM
#undef AUDIO_SANITIZER_ENABLED
#endif

//#if KFW_MODULE_AUDIO_WORKMODE == KFW_MODULE_AUDIO_WORKERRPC_MODE
#undef AUDIO_SANITIZER_ENABLED
//#endif

#ifdef AUDIO_SANITIZER_ENABLED
#define ONLY_AUDIO_ENGINE_THREAD assert(kfw::audio::AudioSanitizer::isEngineThread())
#define ONLY_AUDIO_RPC_THREAD assert(kfw::audio::AudioSanitizer::isEngineThread())
#define ONLY_AUDIO_PROC_THREAD assert(kfw::audio::AudioSanitizer::isEngineThread())
#define ONLY_AUDIO_RPC_OR_PROC_THREAD assert(kfw::audio::AudioSanitizer::isEngineThread())
#define ONLY_AUDIO_MAIN_THREAD assert(kfw::audio::AudioSanitizer::isMainThread())
#define ONLY_AUDIO_MAIN_OR_ENGINE_THREAD assert((kfw::audio::AudioSanitizer::isEngineThread() \
                                                 || kfw::audio::AudioSanitizer::isMainThread()))
#else
#define ONLY_AUDIO_ENGINE_THREAD
#define ONLY_AUDIO_RPC_THREAD
#define ONLY_AUDIO_PROC_THREAD
#define ONLY_AUDIO_RPC_OR_PROC_THREAD
#define ONLY_AUDIO_MAIN_THREAD
#define ONLY_AUDIO_MAIN_OR_ENGINE_THREAD
#endif

#endif // KFW_AUDIO_AUDIOSANITIZER_H
