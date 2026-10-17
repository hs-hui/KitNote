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

#ifndef KN_ENGRAVING_TIEMAP_H
#define KN_ENGRAVING_TIEMAP_H

#include "elementmap.h"

namespace kn::engraving {
class Tie;

//---------------------------------------------------------
//   TieMap
//---------------------------------------------------------

class TieMap : public ElementMap
{
    OBJECT_ALLOCATOR(engraving, TieMap)
public:
    TieMap() {}
    Tie* findNew(Tie* o) const { return (Tie*)(ElementMap::findNew((EngravingItem*)o)); }
    void add(Tie* _o, Tie* _n) { ElementMap::add((EngravingItem*)_o, (EngravingItem*)_n); }
};
} // namespace kn::engraving
#endif
