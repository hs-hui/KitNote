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

#pragma once

#include "async/asyncable.h"
#include "context/iglobalcontext.h"
#include "midi/imidiconfiguration.h"
#include "midi/imidiinport.h"
#include "midi/imidioutport.h"
#include "midiremote/imidiremote.h"
#include "modularity/ioc.h"
#include "notation/inotationconfiguration.h"

namespace kn::notation {
class MidiInputOutputController : public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> configuration;
    kfw::GlobalInject<kfw::midi::IMidiConfiguration> midiConfiguration;
    kfw::ContextInject<kfw::midi::IMidiInPort> midiInPort = { this };
    kfw::ContextInject<kfw::midi::IMidiOutPort> midiOutPort = { this };
    kfw::ContextInject<kfw::midiremote::IMidiRemote> midiRemote  = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:

    MidiInputOutputController(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    void init();

private:
    void checkInputConnection();
    void checkOutputConnection();

    void checkConnection(const kfw::midi::MidiDeviceID& preferredDeviceId, const kfw::midi::MidiDeviceID& currentDeviceId,
                         const kfw::midi::MidiDeviceList& availableDevices, const std::function<kfw::Ret(
                                                                                                     const kfw::midi::MidiDeviceID&)>& connectCallback);

    void onMidiEventReceived(const kfw::midi::tick_t tick, const kfw::midi::Event& event);

    kfw::midi::MidiDeviceID firstAvailableDeviceId(const kfw::midi::MidiDeviceList& devices) const;
};
}
