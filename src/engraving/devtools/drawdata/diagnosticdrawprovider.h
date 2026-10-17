/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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
#ifndef KN_ENGRAVING_DIAGNOSTICDRAWPROVIDER_H
#define KN_ENGRAVING_DIAGNOSTICDRAWPROVIDER_H

#include "idiagnosticdrawprovider.h"
#include "modularity/ioc.h"

namespace kn::engraving {
class DiagnosticDrawProvider : public IDiagnosticDrawProvider, public kfw::Contextable
{
public:
    DiagnosticDrawProvider(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    kfw::Ret generateDrawData(const kfw::io::path_t& dirOrFile, const kfw::io::path_t& outDirOrFile,
                               const GenOpt& opt = GenOpt()) override;
    kfw::Ret compareDrawData(const kfw::io::path_t& ref, const kfw::io::path_t& test, const kfw::io::path_t& outDiff,
                              const ComOpt& opt = ComOpt()) override;
    kfw::Ret drawDataToPng(const kfw::io::path_t& dataFile, const kfw::io::path_t& outFile) override;
    kfw::Ret drawDiffToPng(const kfw::io::path_t& diffFile, const kfw::io::path_t& refFile, const kfw::io::path_t& outFile) override;
};
}

#endif // KN_ENGRAVING_DIAGNOSTICDRAWPROVIDER_H
