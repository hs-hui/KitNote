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

#include "engraving/dom/score.h"
#include "engraving/engravingerrors.h"
#include "io/file.h"

#include "importtef.h"
#include "tableditreader.h"

using namespace kn::iex::tabledit;
using namespace kn::engraving;

kfw::Ret TablEditReader::read(MasterScore* score, const kfw::io::path_t& path, const Options&)
{
    Err err = import(score, path);
    return make_ret(err, path);
}

Err TablEditReader::import(MasterScore* score, const kfw::io::path_t& path, const Options&)
{
    if (!fileSystem()->exists(path)) {
        return Err::FileNotFound;
    }

    kfw::io::File file(path);
    if (!file.open(kfw::io::IODevice::ReadOnly)) {
        return Err::FileOpenError;
    }
    TablEdit tablEdit{ &file, score };
    Err err = tablEdit.import();

    return err;
}
