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

#include "midiremote/imidiremoteconfiguration.h"

namespace kfw::midiremote {
class MidiRemoteConfigurationStub : public IMidiRemoteConfiguration
{
public:
    io::path_t midiMappingUserAppDataPath() const override;

    bool advanceToNextNoteOnKeyRelease() const override;
    void setAdvanceToNextNoteOnKeyRelease(bool value) override;
    kfw::async::Channel<bool> advanceToNextNoteOnKeyReleaseChanged() const override;
};
}
