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

#pragma once

#include "midiremote/imidiremote.h"

namespace kfw::midiremote {
class MidiRemoteStub : public IMidiRemote
{
public:
    const MidiMappingList& midiMappings() const override;
    Ret setMidiMappings(const MidiMappingList& midiMappings) override;
    void resetMidiMappings() override;
    async::Notification midiMappingsChanged() const override;

    // Setting
    void setIsSettingMode(bool arg) override;
    bool isSettingMode() const override;

    void setCurrentActionEvent(const kfw::midi::Event& ev) override;

    // Process
    Ret process(const kfw::midi::Event& ev) override;
};
}
