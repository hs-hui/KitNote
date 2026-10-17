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
#ifndef KFW_DIAGNOSTICS_DIAGNOSTICFILESWRITER_H
#define KFW_DIAGNOSTICS_DIAGNOSTICFILESWRITER_H

#include "modularity/ioc.h"
#include "io/ifilesystem.h"
#include "global/iglobalconfiguration.h"

#include "types/ret.h"
#include "io/path.h"

namespace kfw {
class ZipWriter;
}

namespace kfw::diagnostics {
class DiagnosticFilesWriter : public Contextable
{
    GlobalInject<io::IFileSystem> fileSystem;
    GlobalInject<IGlobalConfiguration> globalConfiguration;

public:
    DiagnosticFilesWriter(const modularity::ContextPtr& iocCtx)
        : Contextable(iocCtx) {}

    kfw::Ret writeDiagnosticFiles(const kfw::io::path_t& destinationZipPath);

private:
    kfw::RetVal<kfw::io::paths_t> scanDir(const std::string& dirName);
    kfw::Ret addFileToZip(const kfw::io::path_t& filePath, kfw::ZipWriter& zip, const std::string& destinationDirName = std::string());
};
}

#endif // KFW_DIAGNOSTICS_DIAGNOSTICFILESWRITER_H
