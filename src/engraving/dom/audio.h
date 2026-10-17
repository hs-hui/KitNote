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

#ifndef KN_ENGRAVING_AUDIO_H
#define KN_ENGRAVING_AUDIO_H

#include "global/allocator.h"
#include "types/bytearray.h"
#include "types/string.h"

namespace kn::engraving {
//---------------------------------------------------------
//   Audio
//---------------------------------------------------------

class Audio
{
    OBJECT_ALLOCATOR(engraving, Audio)

public:
    Audio();
    const kfw::String& path() const { return m_path; }
    void setPath(const kfw::String& s) { m_path = s; }
    const kfw::ByteArray& data() const { return m_data; }
    kfw::ByteArray data() { return m_data; }
    void setData(const kfw::ByteArray& ba) { m_data = ba; }

private:
    kfw::String m_path;
    kfw::ByteArray m_data;
};
} // namespace kn::engraving
#endif // KN_ENGRAVING_AUDIO_H
