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

#ifndef KN_ENGRAVING_STICKING_H
#define KN_ENGRAVING_STICKING_H

#include "textbase.h"

namespace kn::engraving {
//-----------------------------------------------------------------------------
//   @@ Sticking
///    Drum sticking
//-----------------------------------------------------------------------------

class Sticking final : public TextBase
{
    OBJECT_ALLOCATOR(engraving, Sticking)
    DECLARE_CLASSOF(ElementType::STICKING)

    PropertyValue propertyDefault(Pid id) const override;

public:
    Sticking(Segment* parent);

    Sticking* clone() const override { return new Sticking(*this); }

    Segment* segment() const { return (Segment*)explicitParent(); }
    Measure* measure() const { return (Measure*)explicitParent()->explicitParent(); }

    bool isEditAllowed(EditData&) const override;

    bool positionRelativeToNoteheadRest() const override { return true; }
};
} // namespace kn::engraving
#endif
