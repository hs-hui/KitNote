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

#include "engraving/dom/textbase.h"
#include "engraving/rw/xmlwriter.h"

namespace kn::iex::musicxml {
//---------------------------------------------------------
//   MScoreTextToMXML
//---------------------------------------------------------

class MScoreTextToMusicXml
{
public:
    MScoreTextToMusicXml(const kfw::String& tag, const kfw::String& attr, const engraving::CharFormat& defFmt, const kfw::String& mtf);
    static kfw::String toPlainText(const kfw::String& text);
    static kfw::String toPlainTextPlusSymbols(const std::list<engraving::TextFragment>& list);
    static bool split(const std::list<engraving::TextFragment>& in, const int pos, const int len, std::list<engraving::TextFragment>& left,
                      std::list<engraving::TextFragment>& mid, std::list<engraving::TextFragment>& right);
    void writeTextFragments(const std::list<engraving::TextFragment>& fr, engraving::XmlWriter& xml);

private:
    kfw::String updateFormat();
    kfw::String attribs;
    kfw::String tagname;
    engraving::CharFormat oldFormat;
    engraving::CharFormat newFormat;
    kfw::String musicalTextFont;
};
} // namespace kn::iex::musicxml
