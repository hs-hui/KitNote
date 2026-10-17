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
class VoltasPageModel : public AbstractStyleDialogModel
{
    Q_OBJECT

    Q_PROPERTY(kn::notation::StyleItem * voltaPosAbove READ voltaPosAbove CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * voltaHook READ voltaHook CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * voltaLineWidth READ voltaLineWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * voltaLineStyle READ voltaLineStyle CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * voltaDashLineLen READ voltaDashLineLen CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * voltaDashGapLen READ voltaDashGapLen CONSTANT)

    Q_PROPERTY(kn::notation::StyleItem * voltaAlignStartBeforeKeySig READ voltaAlignStartBeforeKeySig CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * voltaAlignEndLeftOfBarline READ voltaAlignEndLeftOfBarline CONSTANT)

    QML_ELEMENT

public:
    explicit VoltasPageModel(QObject* parent = nullptr);

    StyleItem* voltaPosAbove() const;
    StyleItem* voltaHook() const;
    StyleItem* voltaLineWidth() const;
    StyleItem* voltaLineStyle() const;
    StyleItem* voltaDashLineLen() const;
    StyleItem* voltaDashGapLen() const;

    StyleItem* voltaAlignStartBeforeKeySig() const;
    StyleItem* voltaAlignEndLeftOfBarline() const;
};
}
