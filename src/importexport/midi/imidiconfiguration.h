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

#include <optional>

#include "modularity/imoduleinterface.h"
#include "async/channel.h"
#include "io/path.h"

namespace kn::iex::midi {
class IMidiImportExportConfiguration : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IMidiImportExportConfiguration)

public:
    virtual ~IMidiImportExportConfiguration() = default;

    // import
    virtual int midiShortestNote() const = 0; //ticks
    virtual void setMidiShortestNote(int ticks) = 0;
    virtual kfw::async::Channel<int> midiShortestNoteChanged() const = 0;

    virtual bool roundTempo() const = 0;
    virtual void setRoundTempo(bool round) = 0;
    virtual kfw::async::Channel<bool> roundTempoChanged() const = 0;

    virtual void setMidiImportOperationsFile(const std::optional<kfw::io::path_t>& filePath) const = 0;

    // export
    virtual bool isExpandRepeats() const = 0;
    virtual void setExpandRepeats(bool expand) = 0;

    virtual bool isMidiExportRpns() const = 0;
    virtual void setIsMidiExportRpns(bool exportRpns) = 0;
};
}
