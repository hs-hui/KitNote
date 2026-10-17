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

#include "../ireader.h"

namespace kn::engraving {
class Excerpt;
}

namespace kn::engraving::read400  {
class ReadContext;
}

namespace kn::engraving::read114 {
class Read114 : public rw::IReader
{
public:
    //---------------------------------------------------------
    //   read114
    //    import old version <= 1.3 files
    //---------------------------------------------------------
    kfw::Ret readScoreFile(Score* masterScore, XmlReader& e, rw::ReadInOutData* out) override;

    bool pasteStaff(XmlReader& e, Segment* dst, staff_idx_t dstStaff, Fraction scale) override;
    void pasteSymbols(XmlReader& e, ChordRest* dst) override;

    void readTremoloCompat(compat::TremoloCompat* item, XmlReader& xml) override;

    static void setBarLineSpanToStaves(Score*, const read400::ReadContext&);

private:
    void doReadItem(EngravingItem* item, XmlReader& xml) override;

    void readExcerpt(Excerpt* ex, XmlReader& e, read400::ReadContext& ctx);
};
}
