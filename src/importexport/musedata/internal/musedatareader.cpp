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

#include <QFileInfo>

#include "musedatareader.h"

#include "musedata.h"

#include "engraving/dom/masterscore.h"
#include "engraving/engravingerrors.h"

using namespace kn::iex::musedata;
using namespace kn::engraving;

kfw::Ret MuseDataReader::read(MasterScore* score, const kfw::io::path_t& path, const Options&)
{
    if (!QFileInfo::exists(path.toQString())) {
        return make_ret(Err::FileNotFound, path);
    }

    MuseData md(score);
    if (!md.read(path.toQString())) {
        return make_ret(Err::FileUnknownError, path);
    }

    md.convert();
    return kfw::make_ok();
}
