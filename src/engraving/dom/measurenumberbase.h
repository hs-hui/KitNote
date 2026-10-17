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

#ifndef KN_ENGRAVING_MEASURENUMBERBASE_H
#define KN_ENGRAVING_MEASURENUMBERBASE_H

#include "textbase.h"

namespace kn::engraving {
//---------------------------------------------------------
//   MeasureNumberBase
///   The basic element making measure numbers.
///   Reimplemented by MMRestRange
//---------------------------------------------------------

class MeasureNumberBase : public TextBase
{
    OBJECT_ALLOCATOR(engraving, MeasureNumberBase)
public:
    MeasureNumberBase(const ElementType& type, Measure* parent = nullptr, TextStyleType = TextStyleType::DEFAULT);
    MeasureNumberBase(const MeasureNumberBase& other);

    Measure* measure() const { return toMeasure(explicitParent()); }

    bool isEditable() const override { return false; }    // The measure numbers' text should not be editable
    bool positionRelativeToNoteheadRest() const override { return false; }
};
} // namespace kn::engraving

#endif
