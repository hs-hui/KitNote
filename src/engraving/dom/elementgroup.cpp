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

#include "elementgroup.h"
#include "engravingitem.h"

namespace kn::engraving {
void SingleElementGroup::startDrag(EditData& ed)
{
    m_e->startDrag(ed);
}

RectF SingleElementGroup::drag(EditData& ed)
{
    return m_e->drag(ed);
}

void SingleElementGroup::endDrag(EditData& ed)
{
    m_e->endDrag(ed);
    m_e->triggerLayout();
}
} // namespace kn::engraving
