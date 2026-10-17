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

#pragma once

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"

#include "uicomponents/qml/KitNote/UiComponents/abstractmenumodel.h"
#include "notation/notationtypes.h"

namespace kn::notation {
class NotationContextMenuModel : public kfw::uicomponents::AbstractMenuModel
{
    Q_OBJECT
    QML_ELEMENT;

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    Q_INVOKABLE void loadItems(int elementType);

private:
    kfw::uicomponents::MenuItemList makeItemsByElementType(ElementType type);

    kfw::uicomponents::MenuItemList makePageItems();
    kfw::uicomponents::MenuItemList makeDefaultCopyPasteItems();
    kfw::uicomponents::MenuItemList makeMeasureItems();
    kfw::uicomponents::MenuItemList makeStaffTextItems();
    kfw::uicomponents::MenuItemList makeSystemTextItems();
    kfw::uicomponents::MenuItemList makeTimeSignatureItems();
    kfw::uicomponents::MenuItemList makeInstrumentNameItems();
    kfw::uicomponents::MenuItemList makeHarmonyItems();
    kfw::uicomponents::MenuItemList makeFretboardDiagramItems();
    kfw::uicomponents::MenuItemList makeElementInFretBoxItems();
    kfw::uicomponents::MenuItemList makeSelectItems();
    kfw::uicomponents::MenuItemList makeElementItems();
    kfw::uicomponents::MenuItemList makeInsertMeasuresItems();
    kfw::uicomponents::MenuItemList makeDrumPatternItems(); // KitNote
    kfw::uicomponents::MenuItemList makeDrumRudimentItems(); // KitNote
    kfw::uicomponents::MenuItemList makeMoveMeasureItems();
    kfw::uicomponents::MenuItemList makeChangeInstrumentItems();
    kfw::uicomponents::MenuItemList makeVerticalBoxItems();
    kfw::uicomponents::MenuItemList makeHorizontalBoxItems();
    kfw::uicomponents::MenuItemList makeHairpinItems();
    kfw::uicomponents::MenuItemList makeGradualTempoChangeItems();
    kfw::uicomponents::MenuItemList makeTextItems();

    kfw::uicomponents::MenuItem* makeEditStyle(const EngravingItem* element);

    bool isSingleSelection() const;
    bool canSelectSimilarInRange() const;
    bool canSelectSimilar() const;
    bool isDrumsetStaff() const;

    INotationInteractionPtr interaction() const;
    INotationSelectionPtr selection() const;

    const EngravingItem* currentElement() const;

    const INotationInteraction::HitElementContext& hitElementContext() const;
};
}
