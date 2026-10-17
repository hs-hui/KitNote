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

#include "abstractstyledialogmodel.h"

namespace kn::notation {
class HammerOnPullOffTappingPageModel : public AbstractStyleDialogModel
{
    Q_OBJECT

    Q_PROPERTY(kn::notation::StyleItem * showOnStandardStaves READ showOnStandardStaves CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * showOnTabStaves READ showOnTabStaves CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * hopoUpperCase READ hopoUpperCase CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * hopoShowAll READ hopoShowAll CONSTANT)

    Q_PROPERTY(kn::notation::StyleItem * lhTappingSymbolNormalStave READ lhTappingSymbolNormalStave CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * lhTappingSymbolTab READ lhTappingSymbolTab CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * lhTappingShowItemsNormalStave READ lhTappingShowItemsNormalStave CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * lhTappingShowItemsTab READ lhTappingShowItemsTab CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * lhTappingSlurTopAndBottomNoteOnTab READ lhTappingSlurTopAndBottomNoteOnTab CONSTANT)

    Q_PROPERTY(kn::notation::StyleItem * rhTappingSymbolNormalStave READ rhTappingSymbolNormalStave CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * rhTappingSymbolTab READ rhTappingSymbolTab CONSTANT)

    Q_PROPERTY(kn::notation::StyleItem * hopoAlignLettersStandardStaves READ hopoAlignLettersStandardStaves CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * hopoAlignLettersTabStaves READ hopoAlignLettersTabStaves CONSTANT)

    QML_ELEMENT

public:
    explicit HammerOnPullOffTappingPageModel(QObject* parent = nullptr);

    StyleItem* showOnStandardStaves() const;
    StyleItem* showOnTabStaves() const;
    StyleItem* hopoUpperCase() const;
    StyleItem* hopoShowAll() const;

    StyleItem* lhTappingSymbolNormalStave() const;
    StyleItem* lhTappingSymbolTab() const;
    StyleItem* lhTappingShowItemsNormalStave() const;
    StyleItem* lhTappingShowItemsTab() const;
    StyleItem* lhTappingSlurTopAndBottomNoteOnTab() const;

    StyleItem* rhTappingSymbolNormalStave() const;
    StyleItem* rhTappingSymbolTab() const;

    StyleItem* hopoAlignLettersStandardStaves() const;
    StyleItem* hopoAlignLettersTabStaves() const;
};
}
