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

#ifndef KN_PALETTE_PALETTECREATOR_H
#define KN_PALETTE_PALETTECREATOR_H

#include "palettetree.h"

#include "modularity/ioc.h"
#include "ipaletteconfiguration.h"

namespace kn::palette {
class PaletteCreator : public kfw::Contextable
{
    kfw::GlobalInject<IPaletteConfiguration> configuration;

public:

    PaletteCreator(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    PalettePtr newTempoPalette(bool defaultPalette = false);
    PalettePtr newTextPalette(bool defaultPalette = false);
    PalettePtr newTimePalette(bool defaultPalette = false);
    PalettePtr newRepeatsPalette(bool defaultPalette = false);
    PalettePtr newBeamPalette();
    PalettePtr newDynamicsPalette(bool defaultPalette = false);
    PalettePtr newLayoutPalette(bool defaultPalette = false);
    PalettePtr newFingeringPalette(bool defaultPalette = false);
    PalettePtr newTremoloPalette();
    PalettePtr newDrumTechniquesPalette(); // KitNote
    PalettePtr newChartMarksPalette(); // KitNote
    PalettePtr newNoteHeadsPalette();
    PalettePtr newArticulationsPalette(bool defaultPalette = false);
    PalettePtr newOrnamentsPalette(bool defaultPalette = false);
    PalettePtr newAccordionPalette();
    PalettePtr newBracketsPalette();
    PalettePtr newBreathPalette(bool defaultPalette = false);
    PalettePtr newArpeggioPalette();
    PalettePtr newClefsPalette(bool defaultPalette = false);
    PalettePtr newGraceNotePalette();
    PalettePtr newBagpipeEmbellishmentPalette();
    PalettePtr newKeySigPalette();
    PalettePtr newAccidentalsPalette(bool defaultPalette = false);
    PalettePtr newBarLinePalette(bool defaultPalette = false);
    PalettePtr newLinesPalette(bool defaultPalette = false);
    PalettePtr newFretboardDiagramPalette(bool defaultPalette = false);
    PalettePtr newGuitarPalette(bool defaultPalette = false);
    PalettePtr newKeyboardPalette();
    PalettePtr newPitchPalette(bool defaultPalette = false);
    PalettePtr newHarpPalette();
    PalettePtr newHandbellsPalette(bool defaultPalette = false);

    PaletteTreePtr newMasterPaletteTree();
    PaletteTreePtr newDefaultPaletteTree();
};
}

#endif // KN_PALETTE_PALETTECREATOR_H
