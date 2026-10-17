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

#include "global/async/asyncable.h"

#include "global/modularity/ioc.h"
#include "global/iinteractive.h"
#include "actions/iactionsdispatcher.h"
#include "context/iglobalcontext.h"
#include "audio/main/istartaudiocontroller.h"
#include "audio/main/isoundfontcontroller.h"

namespace kn::appjs {
class WebApi : public kfw::async::Asyncable
{
    inline static kfw::GlobalInject<kfw::IInteractive> interactive;
    inline static kfw::GlobalInject<kfw::actions::IActionsDispatcher> dispatcher;
    inline static kfw::GlobalInject<kn::context::IGlobalContext> globalContext;
    inline static kfw::GlobalInject<kfw::audio::IStartAudioController> startAudioController;
    inline static kfw::GlobalInject<kfw::audio::ISoundFontController> soundFontController;

public:

    static WebApi* instance();

    void init();
    void deinit();

    void load(const void* source, unsigned int len);
    void addSoundFont(const std::string& uri);
    void startAudioProcessing();

private:

    WebApi() = default;

    void onProjectSaved(const kfw::io::path_t& path, kn::project::SaveMode mode);

    project::INotationProjectPtr m_currentProject;
};
}
