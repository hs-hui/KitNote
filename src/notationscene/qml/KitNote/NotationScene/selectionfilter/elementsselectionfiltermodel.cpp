/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include "elementsselectionfiltermodel.h"

#include "log.h"
#include "translation.h"

using namespace kn::notation;
using namespace kn::engraving;

ElementsSelectionFilterModel::ElementsSelectionFilterModel(QObject* parent)
    : AbstractSelectionFilterModel(parent)
{
}

void ElementsSelectionFilterModel::loadTypes()
{
    m_types << ElementsSelectionFilterTypes::ALL;

    for (size_t i = 0; i < NUM_ELEMENTS_SELECTION_FILTER_TYPES; i++) {
        m_types << static_cast<ElementsSelectionFilterTypes>(1 << i);
    }
}

QString ElementsSelectionFilterModel::titleForType(const SelectionFilterTypesVariant& variant) const
{
    const ElementsSelectionFilterTypes type = std::get<ElementsSelectionFilterTypes>(variant);

    switch (type) {
    case ElementsSelectionFilterTypes::ALL:
        return kfw::qtrc("notation", "All");
    case ElementsSelectionFilterTypes::DYNAMIC:
        return kfw::qtrc("notation", "Dynamics");
    case ElementsSelectionFilterTypes::HAIRPIN:
        return kfw::qtrc("notation", "Hairpins");
    case ElementsSelectionFilterTypes::FINGERING:
        return kfw::qtrc("notation", "Fingerings");
    case ElementsSelectionFilterTypes::LYRICS:
        return kfw::qtrc("notation", "Lyrics");
    case ElementsSelectionFilterTypes::CHORD_SYMBOL:
        return kfw::qtrc("notation", "Chord symbols");
    case ElementsSelectionFilterTypes::OTHER_TEXT:
        return kfw::qtrc("notation", "Other text");
    case ElementsSelectionFilterTypes::ARTICULATION:
        return kfw::qtrc("notation", "Articulations");
    case ElementsSelectionFilterTypes::ORNAMENT:
        return kfw::qtrc("notation", "Ornaments");
    case ElementsSelectionFilterTypes::SLUR:
        return kfw::qtrc("notation", "Slurs");
    case ElementsSelectionFilterTypes::TIE:
        return kfw::qtrc("notation", "Ties");
    case ElementsSelectionFilterTypes::FIGURED_BASS:
        return kfw::qtrc("notation", "Figured bass");
    case ElementsSelectionFilterTypes::OTTAVA:
        return kfw::qtrc("notation", "Ottavas");
    case ElementsSelectionFilterTypes::PEDAL_LINE:
        return kfw::qtrc("notation", "Pedal lines");
    case ElementsSelectionFilterTypes::OTHER_LINE:
        return kfw::qtrc("notation", "Other lines");
    case ElementsSelectionFilterTypes::ARPEGGIO:
        return kfw::qtrc("notation", "Arpeggios");
    case ElementsSelectionFilterTypes::GLISSANDO:
        return kfw::qtrc("notation", "Glissandos");
    case ElementsSelectionFilterTypes::FRET_DIAGRAM:
        return kfw::qtrc("notation", "Fretboard diagrams");
    case ElementsSelectionFilterTypes::BREATH:
        return kfw::qtrc("notation", "Breath marks");
    case ElementsSelectionFilterTypes::TREMOLO:
        return kfw::qtrc("notation", "Tremolos");
    case ElementsSelectionFilterTypes::GRACE_NOTE:
        return kfw::qtrc("notation", "Grace notes");
    case ElementsSelectionFilterTypes::NONE:
        break;
    }

    return {};
}
