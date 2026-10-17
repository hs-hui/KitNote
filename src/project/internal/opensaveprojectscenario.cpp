/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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

#include "opensaveprojectscenario.h"

#include "engraving/infrastructure/mscio.h"
#include "projecterrors.h"

using namespace kfw;
using namespace kn::project;

RetVal<SaveLocation> OpenSaveProjectScenario::askSaveLocation(INotationProjectPtr project, SaveMode mode) const
{
    configuration()->setLastUsedSaveLocationType(SaveLocationType::Local);

    RetVal<kfw::io::path_t> path = askLocalPath(project, mode);
    if (!path.ret) {
        return path.ret;
    }

    return RetVal<SaveLocation>::make_ok(SaveLocation(path.val));
}

RetVal<kfw::io::path_t> OpenSaveProjectScenario::askLocalPath(INotationProjectPtr project, SaveMode saveMode) const
{
    std::string dialogTitle = kfw::trc("project/save", "Save score");
    std::string filenameAddition;

    if (saveMode == SaveMode::SaveCopy) {
        //: used to form a filename suggestion, like "originalFile - copy"
        filenameAddition = " - " + kfw::trc("project/save", "copy", "a copy of a file");
    } else if (saveMode == SaveMode::SaveSelection) {
        //: used to form a filename suggestion, like "originalFile - selection"
        filenameAddition = " - " + kfw::trc("project/save", "selection");
    }

    kfw::io::path_t defaultPath = configuration()->defaultSavingFilePath(project, filenameAddition);

    std::vector<std::string> filter {
        kfw::trc("project", "KitNote file") + " (*.kitnote)",
        kfw::trc("project", "MuseScore file") + " (*.mscz)",
        kfw::trc("project", "Uncompressed MuseScore folder (experimental)")
#ifdef Q_OS_MAC
        + " (*)"
#else
        + " (*.)"
#endif
    };

    kfw::io::path_t selectedPath = interactive()->selectSavingFileSync(dialogTitle, defaultPath, filter);

    if (selectedPath.empty()) {
        return make_ret(Ret::Code::Cancel);
    }

    if (!engraving::isKitNoteFile(io::suffix(selectedPath))) {
        // Then it must be that the user is trying to save a mscx file.
        // At the selected path, a folder will be created,
        // and inside the folder, a mscx file will be created.
        // We should return the path to the mscx file.
        selectedPath = selectedPath.appendingComponent(io::filename(selectedPath)).appendingSuffix(engraving::MSCX);
    }

    configuration()->setLastSavedProjectsPath(io::dirpath(selectedPath));

    return RetVal<kfw::io::path_t>::make_ok(selectedPath);
}
