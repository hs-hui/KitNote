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

#include <vector>

#include "../iconvertercontroller.h"

#include "modularity/ioc.h"
#include "project/iprojectcreator.h"
#include "project/inotationwritersregister.h"
#include "project/iprojectrwregister.h"
#include "context/iglobalcontext.h"

#include "types/retval.h"

namespace kn::converter {
class ConverterController : public IConverterController, public kfw::Contextable
{
    kfw::GlobalInject<project::IProjectCreator> notationCreator;
    kfw::ContextInject<project::INotationWritersRegister> writers = { this };
    kfw::ContextInject<project::IProjectRWRegister> projectRW = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    ConverterController(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    kfw::Ret fileConvert(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {},
                          const kfw::String& soundProfile = {}, const kfw::io::path_t& tracksDiffPath = {},
                          const std::string& transposeOptionsJson = {},
                          const std::optional<ConvertTarget>& target = std::nullopt) override;

    kfw::Ret batchConvert(const kfw::io::path_t& batchJobFile, const OpenParams& openParams = {}, const kfw::String& soundProfile = {},
                           kfw::ProgressPtr progress = nullptr) override;

    kfw::Ret convertScoreParts(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {}) override;

    kfw::Ret exportScoreMedia(const kfw::io::path_t& in, const kfw::io::path_t& out,  const OpenParams& openParams = {},
                               const kfw::io::path_t& highlightConfigPath = kfw::io::path_t()) override;
    kfw::Ret exportScoreMeta(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {}) override;
    kfw::Ret exportScoreParts(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {}) override;
    kfw::Ret exportScorePartsPdfs(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {}) override;
    kfw::Ret exportScoreTranspose(const kfw::io::path_t& in, const kfw::io::path_t& out, const std::string& optionsJson,
                                   const OpenParams& openParams = {}) override;

    kfw::Ret exportScoreElements(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {}) override;

    kfw::Ret updateSource(const kfw::io::path_t& in, const std::string& newSource, bool forceMode = false) override;

private:
    struct CopyrightInfo {
        CopyrightInfo() {}

        kfw::String text;
        bool showOnAllPages = false;
    };

    struct Job {
        kfw::io::path_t in;
        kfw::io::path_t out;
        kfw::io::path_t tracksDiffPath;
        std::optional<notation::TransposeOptions> transposeOptions;
        std::optional<size_t> pageNum;
        std::vector<size_t> visibleParts;
        CopyrightInfo copyright;
    };

    using BatchJob = std::vector<Job>;
    using TransposeOpts = std::optional<notation::TransposeOptions>;

    kfw::RetVal<BatchJob> parseBatchJob(const kfw::io::path_t& batchJobFile) const;

    kfw::Ret convertFile(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {},
                          const kfw::String& soundProfile = {}, const kfw::io::path_t& tracksDiffPath = {},
                          const TransposeOpts& transposeOptions = std::nullopt,
                          const std::optional<ConvertTarget>& target = std::nullopt, const std::vector<size_t>& visibleParts = {},
                          const CopyrightInfo& copyright = {});

    kfw::Ret convertScoreParts(project::INotationWriterPtr writer, notation::IMasterNotationPtr masterNotation,
                                const kfw::io::path_t& out);

    bool isConvertPageByPage(const std::string& suffix) const;
    kfw::Ret convertPageByPage(project::INotationWriterPtr writer, notation::INotationPtr notation, const kfw::io::path_t& out) const;
    kfw::Ret convertPage(project::INotationWriterPtr writer, notation::INotationPtr notation, const size_t pageNum,
                          const kfw::io::path_t& filePath, const kfw::io::path_t& dirPath = {}) const;
    kfw::Ret convertFullNotation(project::INotationWriterPtr writer, notation::INotationPtr notation, const kfw::io::path_t& out,
                                  const project::INotationWriter::Options& options = {}) const;

    kfw::Ret convertScorePartsToPdf(project::INotationWriterPtr writer, notation::IMasterNotationPtr masterNotation,
                                     const kfw::io::path_t& out) const;
    kfw::Ret convertScorePartsToPngs(project::INotationWriterPtr writer, notation::IMasterNotationPtr masterNotation,
                                      const kfw::io::path_t& out) const;
    kfw::Ret convertScorePartsToMp3(project::INotationWriterPtr writer, notation::IMasterNotationPtr masterNotation,
                                     const kfw::io::path_t& out) const;

    kfw::Ret saveRegion(project::INotationProjectPtr project, const ConvertRegionJson& regionJson, const kfw::io::path_t& out) const;

    kfw::Ret writeTracksDiff(project::INotationProjectPtr project, const QJsonArray& oldTracks, const kfw::io::path_t& path) const;
};
}
