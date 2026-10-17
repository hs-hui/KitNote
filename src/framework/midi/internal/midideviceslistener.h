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
#ifndef KFW_MIDI_MIDIDEVICESLISTENER_H
#define KFW_MIDI_MIDIDEVICESLISTENER_H

#include <thread>
#include <mutex>
#include <condition_variable>

#include "async/notification.h"
#include "miditypes.h"

namespace kfw::midi {
class MidiDevicesListener
{
public:
    ~MidiDevicesListener();

    using ActualDevicesCallback = std::function<MidiDeviceList ()>;

    void startWithCallback(const ActualDevicesCallback& callback);

    async::Notification devicesChanged() const;

private:
    void th_updateDevices();
    void th_setDevices(const MidiDeviceList& devices);
    void stop();

    std::shared_ptr<std::thread> m_devicesUpdateThread;
    std::atomic<bool> m_isRunning = false;

    mutable std::mutex m_mutex;
    std::condition_variable m_runningCv;

    MidiDeviceList m_devices;
    async::Notification m_devicesChanged;

    ActualDevicesCallback m_actualDevicesCallback;
};
}

#endif // KFW_MIDI_MIDIDEVICESLISTENER_H
