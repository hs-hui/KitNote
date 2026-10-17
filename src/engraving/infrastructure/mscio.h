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
#ifndef KN_ENGRAVING_MSCIO_H
#define KN_ENGRAVING_MSCIO_H

#include <string>

#include "io/path.h"

namespace kn::engraving {
//! NOTE The main format is MuseScore, is a zip archive with a specific structure
static const std::string MSCZ = "mscz";

//! NOTE This is the automatically generated backup file.
static const std::string MSCZ_BACKUP = "mscz~";

//! NOTE KitNote: KitNote's own extension. The content is exactly a `.mscz`
//! archive, so renaming a file between the two extensions is all it takes to
//! move a score between KitNote and MuseScore Studio.
static const std::string KITNOTE = "kitnote";
static const std::string KITNOTE_BACKUP = "kitnote~";

//! The compressed single-file formats, i.e. the ones that get backups and whose
//! extension is not worth showing next to the score's name.
inline bool isCompressedScoreSuffix(const std::string& suffix)
{
    return suffix == MSCZ || suffix == KITNOTE;
}

//! NOTE Before MuseScore 4, MuseScore could save data in one xml file (excluding binary) with `.mscx` extension.
//! Starting from MuseScore 4, only the score domain model store to the `.mscx` file,
//! and other data, such as styles, chordlist, synthesizer settings, etc. are stored in separate files.
//! We can choose to save a `.mscx` file, in this case a folder with the same structure
//! as a zip container will be created (i.e. an unpacked zip container)
//! We can select `.mscx` to open, in this case it is now expected that it is in a folder
//! in which there are also other data, structured as in a zip container.
static const std::string MSCX = "mscx";

//! NOTE For testing purposes and some workflows of work with score, there is a need to store all data in one text file.
//! Therefore, a new file type has been introduced for this.
//! This is a XML file that stores all files from the ZIP container by concatenation (excluding binaries)
static const std::string MSCS = "mscs";

inline bool isKitNoteFile(const std::string& suffix)
{
    return suffix == MSCZ || suffix == MSCX || suffix == MSCS || suffix == MSCZ_BACKUP
           || suffix == KITNOTE || suffix == KITNOTE_BACKUP;
}

enum class MscIoMode : unsigned char {
    Unknown = 0,
    Zip,
    Dir,
    XmlFile
};

inline MscIoMode mscIoModeBySuffix(const std::string& suffix)
{
    if (suffix == MSCZ || suffix == MSCZ_BACKUP || suffix == KITNOTE || suffix == KITNOTE_BACKUP) {
        return MscIoMode::Zip;
    } else if (suffix == MSCX) {
        return MscIoMode::Dir;
    } else if (suffix == MSCS) {
        return MscIoMode::XmlFile;
    }
    return MscIoMode::Unknown;
}

inline kfw::io::path_t containerPath(const kfw::io::path_t& path)
{
    if (kfw::io::suffix(path) == MSCX) {
        return kfw::io::absoluteDirpath(path);
    }

    return path;
}

inline kfw::io::path_t mainFilePath(const kfw::io::path_t& path)
{
    if (isKitNoteFile(kfw::io::suffix(path))) {
        return path;
    }

    return path.appendingComponent(kfw::io::filename(path)).appendingSuffix(MSCX);
}

inline kfw::io::path_t mainFileName(const kfw::io::path_t& path)
{
    return kfw::io::filename(path, !isKitNoteFile(kfw::io::suffix(path))).appendingSuffix(MSCX);
}
}

#endif // KN_ENGRAVING_MSCIO_H
