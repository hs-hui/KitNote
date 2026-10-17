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
#ifndef KN_PROJECT_EXPORTPROJECTSCENARIO_H
#define KN_PROJECT_EXPORTPROJECTSCENARIO_H

#include "modularity/ioc.h"

#include "iexportprojectscenario.h"
#include "iprojectconfiguration.h"
#include "iinteractive.h"
#include "inotationwritersregister.h"
#include "importexport/imagesexport/iimagesexportconfiguration.h"
#include "context/iglobalcontext.h"
#include "io/ifilesystem.h"
#include "async/asyncable.h"

namespace kn::project {
class ExportProjectScenario : public IExportProjectScenario, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;
    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::GlobalInject<iex::imagesexport::IImagesExportConfiguration> imagesExportConfiguration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<INotationWritersRegister> writers = { this };
    kfw::ContextInject<context::IGlobalContext> context = { this };

public:

    ExportProjectScenario(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    std::vector<INotationWriter::UnitType> supportedUnitTypes(const ExportType& exportType) const override;

    kfw::RetVal<kfw::io::path_t> askExportPath(const notation::INotationPtrList& notations, const ExportType& exportType,
                                                 INotationWriter::UnitType unitType = INotationWriter::UnitType::PER_PART,
                                                 kfw::io::path_t defaultPath = "") const override;

    bool exportScores(notation::INotationPtrList notations, const kfw::io::path_t destinationPath,
                      INotationWriter::UnitType unitType = INotationWriter::UnitType::PER_PART,
                      bool openDestinationFolderOnExport = false) const override;

    const ExportInfo& exportInfo() const override;
    void setExportInfo(const ExportInfo& exportInfo) override;

private:
    enum class FileConflictPolicy {
        Undefined,
        SkipAll,
        ReplaceAll
    };

    /// When the user is trying to export a part that is a "potential excerpt", the corresponding score
    /// is not initialized yet, so we can't be certain about the page count. We should not initialize
    /// these scores either, until the user really starts the export, because initializing these scores
    /// means making changes to the file, which can't be done without the user's consent.
    bool guessIsCreatingOnlyOneFile(const notation::INotationPtrList& notations, INotationWriter::UnitType unitType) const;
    size_t exportFileCount(const notation::INotationPtrList& notations, INotationWriter::UnitType unitType) const;

    bool isMainNotation(notation::INotationPtr notation) const;
    notation::IMasterNotationPtr masterNotation() const;

    kfw::io::path_t completeExportPath(const kfw::io::path_t& basePath, notation::INotationPtr notation, bool isMain,
                                        bool isExportingOnlyOneScore, int pageIndex = -1) const;

    bool shouldReplaceFile(const QString& filename) const;
    bool askForRetry(const QString& filename) const;

    kfw::Ret doExportLoop(const kfw::io::path_t& path, std::function<kfw::Ret(kfw::io::IODevice&)> exportFunction) const;

    void showExportProgress(bool isAudioExport) const;

    void openFolder(const kfw::io::path_t& path) const;

    std::vector<notation::ViewMode> viewModes(const notation::INotationPtrList& notations) const;
    void setViewModes(const notation::INotationPtrList& notations, const std::vector<notation::ViewMode>& viewModes) const;
    void setViewModes(const notation::INotationPtrList& notations, notation::ViewMode viewMode) const;

    mutable FileConflictPolicy m_fileConflictPolicy = FileConflictPolicy::Undefined;
    mutable kfw::Progress m_exportProgress;
    ExportInfo m_exportInfo;
};
}

#endif // KN_PROJECT_EXPORTPROJECTSCENARIO_H
