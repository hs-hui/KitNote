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

#ifndef KN_ENGRAVING_ELEMENTMAP_H
#define KN_ENGRAVING_ELEMENTMAP_H

#include <unordered_map>

#include "global/allocator.h"

namespace kn::engraving {
class EngravingItem;

//---------------------------------------------------------
//   ElementMap
//---------------------------------------------------------

class ElementMap : public std::unordered_map<EngravingItem*, EngravingItem*>
{
    OBJECT_ALLOCATOR(engraving, ElementMap)
public:
    ElementMap() {}
    EngravingItem* findNew(EngravingItem* o) const;
    void add(EngravingItem* o, EngravingItem* n);
};
} // namespace kn::engraving
#endif
