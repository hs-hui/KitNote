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

#ifndef KN_ENGRAVING_NOTEDOT_H
#define KN_ENGRAVING_NOTEDOT_H

#include "engravingitem.h"

namespace kn::engraving {
class Factory;
class Note;
class Rest;

//---------------------------------------------------------
//   @@ NoteDot
//---------------------------------------------------------

class NoteDot final : public EngravingItem
{
    OBJECT_ALLOCATOR(engraving, NoteDot)
    DECLARE_CLASSOF(ElementType::NOTEDOT)

public:

    NoteDot* clone() const override { return new NoteDot(*this); }
    double mag() const override;

    Note* note() const { return explicitParent()->isNote() ? toNote(explicitParent()) : 0; }
    Rest* rest() const { return explicitParent()->isRest() ? toRest(explicitParent()) : 0; }
    EngravingItem* elementBase() const override;

private:
    friend class Factory;
    NoteDot(Note* parent);
    NoteDot(Rest* parent);
};
} // namespace kn::engraving
#endif
