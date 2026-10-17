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
#ifndef KFW_MIDI_IMIDIINPORT_H
#define KFW_MIDI_IMIDIINPORT_H

#include "modularity/imoduleinterface.h"

#include "types/ret.h"
#include "async/channel.h"
#include "async/notification.h"
#include "miditypes.h"

namespace kfw::midi {
class IMidiInPort : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IMidiInPort)

public:
    virtual ~IMidiInPort() = default;

    virtual MidiDeviceList availableDevices() const = 0;
    virtual async::Notification availableDevicesChanged() const = 0;

    virtual Ret connect(const MidiDeviceID& deviceID) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual MidiDeviceID deviceID() const = 0;
    virtual async::Notification deviceChanged() const = 0;

    virtual async::Channel<tick_t, Event> eventReceived() const = 0;
};
}

#endif // KFW_MIDI_IMIDIINPORT_H
