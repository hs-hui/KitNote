/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include <QFile>

#include "types/retval.h"

#include "io/path.h"

#include "modularity/ioc.h"
#include "global/iapplication.h"
#include "io/ifilesystem.h"
#include "project/iprojectcreator.h"
#include "project/inotationwritersregister.h"

#include "converter/convertertypes.h"

namespace kn::engraving {
class Score;
}

namespace kn::converter {
class BackendJsonWriter;
class BackendApi
{
    inline static kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;
    inline static kfw::GlobalInject<project::IProjectCreator> notationCreator;
    inline static kfw::ContextInject<kfw::IApplication> application = { nullptr }; // FIXME
    inline static kfw::ContextInject<project::INotationWritersRegister> writers = { nullptr }; // FIXME

public:
    static kfw::Ret exportScoreMedia(const kfw::io::path_t& in, const kfw::io::path_t& out, const kfw::io::path_t& highlightConfigPath,
                                      const OpenParams& openParams = {});
    static kfw::Ret exportScoreMeta(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {});
    static kfw::Ret exportScoreParts(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {});
    static kfw::Ret exportScorePartsPdfs(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {});
    static kfw::Ret exportScoreTranspose(const kfw::io::path_t& in, const kfw::io::path_t& out, const std::string& optionsJson,
                                          const OpenParams& openParams = {});

    static kfw::Ret exportScoreElements(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {});

    static kfw::Ret updateSource(const kfw::io::path_t& in, const std::string& newSource, bool forceMode = false);

private:
    static kfw::Ret openOutputFile(QFile& file, const kfw::io::path_t& out);

    static kfw::RetVal<project::INotationProjectPtr> openProject(const kfw::io::path_t& path, const OpenParams& params = {});

    static QVariantMap readBeatsColors(const kfw::io::path_t& filePath);

    static kfw::Ret exportScorePngs(const notation::INotationPtr notation, BackendJsonWriter& jsonWriter, bool addSeparator = false);
    static kfw::Ret exportScoreSvgs(const notation::INotationPtr notation, const kfw::io::path_t& highlightConfigPath,
                                     BackendJsonWriter& jsonWriter, bool addSeparator = false);
    static kfw::Ret exportScoreElementsPositions(const std::string& elementsPositionsWriterName,
                                                  const std::string& elementsPositionsTagName, const notation::INotationPtr notation,
                                                  BackendJsonWriter& jsonWriter, bool addSeparator = false);
    static kfw::Ret exportScorePdf(const notation::INotationPtr notation, BackendJsonWriter& jsonWriter, bool addSeparator = false);
    static kfw::Ret exportScorePdf(const notation::INotationPtr notation, QIODevice& destinationDevice);
    static kfw::Ret exportScoreMidi(const notation::INotationPtr notation, BackendJsonWriter& jsonWriter, bool addSeparator = false);
    static kfw::Ret exportScoreMusicXML(const notation::INotationPtr notation, BackendJsonWriter& jsonWriter, bool addSeparator = false);
    static kfw::Ret exportScoreMetaData(const notation::INotationPtr notation, BackendJsonWriter& jsonWriter, bool addSeparator = false);
    static kfw::Ret devInfo(const notation::INotationPtr notation, BackendJsonWriter& jsonWriter, bool addSeparator = false);

    static kfw::RetVal<QByteArray> processWriter(const std::string& writerName, const notation::INotationPtr notation);
    static kfw::RetVal<QByteArray> processWriter(const std::string& writerName, const notation::INotationPtrList notations,
                                                  const project::INotationWriter::Options& options);

    static kfw::Ret doExportScoreParts(const notation::IMasterNotationPtr notation, QIODevice& destinationDevice);
    static kfw::Ret doExportScorePartsPdfs(const notation::IMasterNotationPtr notation, QIODevice& destinationDevice,
                                            const std::string& scoreFileName);
    static kfw::Ret doExportScoreTranspose(const notation::INotationPtr notation, BackendJsonWriter& jsonWriter,
                                            bool addSeparator = false);

    static kfw::Ret doExportScoreElements(const notation::INotationPtr notation, QIODevice& out);

    static kfw::RetVal<QByteArray> scorePartJson(kn::engraving::Score* score, const std::string& fileName);

    static void switchToPageView(notation::IMasterNotationPtr masterNotation);
    static void renderExcerptsContents(notation::IMasterNotationPtr masterNotation);

    static notation::ExcerptNotationList allExcerpts(notation::IMasterNotationPtr masterNotation);
    static void initPotentialExcerpts(notation::IMasterNotationPtr masterNotation);
};
}
