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

#include "modularity/imoduleinterface.h"

#include "async/channel.h"
#include "io/path.h"

namespace kfw::midiremote {
class IMidiRemoteConfiguration : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IMidiRemoteConfiguration)

public:
    virtual ~IMidiRemoteConfiguration() = default;

    virtual io::path_t midiMappingUserAppDataPath() const = 0;

    virtual bool advanceToNextNoteOnKeyRelease() const = 0;
    virtual void setAdvanceToNextNoteOnKeyRelease(bool value) = 0;
    virtual async::Channel<bool> advanceToNextNoteOnKeyReleaseChanged() const = 0;
};
}
