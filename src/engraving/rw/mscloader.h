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

#include "global/types/ret.h"

#include "../infrastructure/mscreader.h"

namespace kn::engraving::compat {
class ReadStyleHook;
}

namespace kn::engraving::rw {
struct ReadInOutData;
}

namespace kn::engraving {
class MasterScore;
class XmlReader;
class MscLoader
{
public:
    MscLoader() = default;

    kfw::Ret loadMscz(MasterScore* score, const MscReader& mscReader, rw::ReadInOutData* out, bool ignoreVersionError);

private:
    friend class MasterScore;
    kfw::Ret readMasterScore(MasterScore* score, XmlReader&, bool ignoreVersionError, rw::ReadInOutData* out = nullptr,
                              compat::ReadStyleHook* styleHook = nullptr);
};
}
