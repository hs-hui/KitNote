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
#ifndef KFW_MIDI_COREMIDIINPORT_H
#define KFW_MIDI_COREMIDIINPORT_H

#include <memory>

#include "imidiinport.h"

namespace kfw::midi {
class CoreMidiInPort : public IMidiInPort
{
public:
    CoreMidiInPort();
    ~CoreMidiInPort() override;

    void init();
    void deinit();

    MidiDeviceList availableDevices() const override;
    async::Notification availableDevicesChanged() const override;

    Ret connect(const MidiDeviceID& deviceID) override;
    void disconnect() override;
    bool isConnected() const override;
    MidiDeviceID deviceID() const override;
    async::Notification deviceChanged() const override;

    async::Channel<tick_t, Event> eventReceived() const override;

private:
    Ret run();
    void stop();

    void initCore();

    struct Core;
    std::unique_ptr<Core> m_core;
    MidiDeviceID m_deviceID;
    async::Notification m_deviceChanged;
    async::Notification m_availableDevicesChanged;

    bool m_running = false;

    async::Channel<tick_t, Event > m_eventReceived;
};
}

#endif // KFW_MIDI_COREMIDIINPORT_H
