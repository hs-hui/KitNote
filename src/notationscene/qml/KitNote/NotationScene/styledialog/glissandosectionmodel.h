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
class GlissandoSectionModel : public AbstractStyleDialogModel
{
    Q_OBJECT

    Q_PROPERTY(kn::notation::StyleItem * glissandoLineStyle READ glissandoLineStyle CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * glissandoLineStyleDashSize READ glissandoLineStyleDashSize CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * glissandoLineStyleGapSize READ glissandoLineStyleGapSize CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * glissandoLineWidth READ glissandoLineWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * glissandoShowText READ glissandoShowText CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * glissandoText READ glissandoText CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * glissandoLineType READ glissandoLineType CONSTANT)

    QML_ELEMENT

public:
    explicit GlissandoSectionModel(QObject* parent = nullptr);

    StyleItem* glissandoLineStyle() const;
    StyleItem* glissandoLineStyleDashSize() const;
    StyleItem* glissandoLineStyleGapSize() const;
    StyleItem* glissandoLineWidth() const;
    StyleItem* glissandoShowText() const;
    StyleItem* glissandoText() const;
    StyleItem* glissandoLineType() const;
};
}
