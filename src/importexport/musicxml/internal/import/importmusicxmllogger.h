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

#include "serialization/xmlstreamreader.h"

namespace kn::iex::musicxml {
class MusicXmlLogger
{
public:
    enum class Level : char {
        MXML_TRACE, MXML_INFO, MXML_ERROR
    };
    MusicXmlLogger() {}
    void logDebugTrace(const kfw::String& trace, const kfw::XmlStreamReader* xmlreader = 0);
    void logDebugInfo(const kfw::String& info, const kfw::XmlStreamReader* xmlreader = 0);
    void logError(const kfw::String& error, const kfw::XmlStreamReader* xmlreader = 0);
    void setLoggingLevel(const Level level) { m_level = level; }
private:
    Level m_level = Level::MXML_INFO;
};
} // namespace kn::iex::musicxml
