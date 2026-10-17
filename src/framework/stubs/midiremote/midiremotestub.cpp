/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2026 MuseScore Limited and others
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
#include "midiremotestub.h"

using namespace kfw;
using namespace kfw::midiremote;

const MidiMappingList& MidiRemoteStub::midiMappings() const
{
    static MidiMappingList l;
    return l;
}

Ret MidiRemoteStub::setMidiMappings(const MidiMappingList&)
{
    return kfw::make_ret(Ret::Code::NotImplemented);
}

void MidiRemoteStub::resetMidiMappings()
{
}

async::Notification MidiRemoteStub::midiMappingsChanged() const
{
    return async::Notification();
}

void MidiRemoteStub::setIsSettingMode(bool)
{
}

bool MidiRemoteStub::isSettingMode() const
{
    return false;
}

void MidiRemoteStub::setCurrentActionEvent(const kfw::midi::Event&)
{
}

Ret MidiRemoteStub::process(const kfw::midi::Event&)
{
    return kfw::make_ret(Ret::Code::NotImplemented);
}
