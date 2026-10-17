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

#ifndef KN_ENGRAVING_FINGERING_H
#define KN_ENGRAVING_FINGERING_H

#include "textbase.h"

#include "../types/types.h"

namespace kn::engraving {
class Note;

//---------------------------------------------------------
//   @@ Fingering
//---------------------------------------------------------

class Fingering final : public TextBase
{
    OBJECT_ALLOCATOR(engraving, Fingering)
    DECLARE_CLASSOF(ElementType::FINGERING)

public:
    Fingering(Note* parent, TextStyleType tid, ElementFlags ef = ElementFlag::ON_STAFF);
    Fingering(Note* parent, ElementFlags ef = ElementFlag::ON_STAFF);

    Fingering* clone() const override { return new Fingering(*this); }

    Note* note() const { return toNote(explicitParent()); }
    ElementType layoutType() const;
    PlacementV calculatePlacement() const;

    bool isEditAllowed(EditData&) const override;

    PropertyValue propertyDefault(Pid id) const override;

    String accessibleInfo() const override;

    bool isOnCrossBeamSide() const;

    bool positionRelativeToNoteheadRest() const override { return false; }
};
} // namespace kn::engraving
#endif
