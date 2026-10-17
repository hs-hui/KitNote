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
#ifndef KN_ENGRAVING_IDIAGNOSTICDRAWPROVIDER_H
#define KN_ENGRAVING_IDIAGNOSTICDRAWPROVIDER_H

#include "modularity/imoduleinterface.h"
#include "global/types/ret.h"
#include "global/io/path.h"

#include "drawdatatypes.h"

namespace kn::engraving {
class IDiagnosticDrawProvider : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IDiagnosticDrawProvider)
public:
    virtual ~IDiagnosticDrawProvider() = default;

    virtual kfw::Ret generateDrawData(const kfw::io::path_t& scoresDir, const kfw::io::path_t& outDir, const GenOpt& opt = GenOpt()) = 0;
    virtual kfw::Ret compareDrawData(const kfw::io::path_t& ref, const kfw::io::path_t& test, const kfw::io::path_t& outDiff,
                                      const ComOpt& opt = ComOpt()) = 0;
    virtual kfw::Ret drawDataToPng(const kfw::io::path_t& dataFile, const kfw::io::path_t& outFile) = 0;
    virtual kfw::Ret drawDiffToPng(const kfw::io::path_t& diffFile, const kfw::io::path_t& refFile, const kfw::io::path_t& outFile) = 0;
};
}

#endif // KN_ENGRAVING_IDIAGNOSTICDRAWPROVIDER_H
