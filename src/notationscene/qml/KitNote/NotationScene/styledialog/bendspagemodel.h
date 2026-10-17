/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2026 MuseScore Limited and others
 * Modified for KitNote, 2026
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "abstractstyledialogmodel.h"

namespace kn::notation {
class BendsPageModel : public AbstractStyleDialogModel
{
    Q_OBJECT

    Q_PROPERTY(kn::notation::StyleItem * guitarBendUseFull READ guitarBendUseFull CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * guitarBendLineWidth READ guitarBendLineWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * guitarBendLineWidthTab READ guitarBendLineWidthTab CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * guitarBendArrowWidth READ guitarBendArrowWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * guitarBendArrowHeight READ guitarBendArrowHeight CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * guitarDivesAboveStaff READ guitarDivesAboveStaff CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * useCueSizeFretForGraceBends READ useCueSizeFretForGraceBends CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * useFractionCharacters READ useFractionCharacters CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * alignPreBendAndPreDiveToGraceNote READ alignPreBendAndPreDiveToGraceNote CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * guitarDiveLineWidth READ guitarDiveLineWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * guitarDiveLineWidthTab READ guitarDiveLineWidthTab CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * whammyBarText READ whammyBarText CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * whammyBarLineStyle READ whammyBarLineStyle CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * whammyBarDashLineLen READ whammyBarDashLineLen CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * whammyBarDashGapLen READ whammyBarDashGapLen CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * whammyBarLineWidth READ whammyBarLineWidth CONSTANT)

    QML_ELEMENT

public:
    explicit BendsPageModel(QObject* parent = nullptr);

    StyleItem* guitarBendUseFull() const;
    StyleItem* guitarBendLineWidth() const;
    StyleItem* guitarBendLineWidthTab() const;
    StyleItem* guitarBendArrowWidth() const;
    StyleItem* guitarBendArrowHeight() const;
    StyleItem* guitarDivesAboveStaff() const;
    StyleItem* useCueSizeFretForGraceBends() const;
    StyleItem* useFractionCharacters() const;
    StyleItem* alignPreBendAndPreDiveToGraceNote() const;
    StyleItem* guitarDiveLineWidth() const;
    StyleItem* guitarDiveLineWidthTab() const;
    StyleItem* whammyBarText() const;
    StyleItem* whammyBarLineStyle() const;
    StyleItem* whammyBarDashLineLen() const;
    StyleItem* whammyBarDashGapLen() const;
    StyleItem* whammyBarLineWidth() const;
};
}
