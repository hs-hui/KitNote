/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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

#include "project/inotationwriter.h"

namespace kn::iex::midi {
//! KitNote: writes a score as a text drum tab (see INotationInteraction::drumTabText) - "KitNote -o
//! song.txt song.kitnote" on the command line, and "Drum tab" in the export dialog. It lives in this
//! module only because the module is there: it has nothing to do with MIDI.
class DrumTabWriter : public project::INotationWriter
{
public:
    std::vector<UnitType> supportedUnitTypes() const override;
    bool supportsUnitType(UnitType unitType) const override;

    kfw::Ret write(notation::INotationPtr notation, kfw::io::IODevice& dstDevice, const Options& options = Options()) override;
    kfw::Ret writeList(const notation::INotationPtrList& notations, kfw::io::IODevice& dstDevice,
                        const Options& options = Options()) override;
};
}
