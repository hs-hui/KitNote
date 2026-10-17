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

#pragma once

#include "abstractstyledialogmodel.h"

namespace kn::notation {
class SlursAndTiesPageModel : public AbstractStyleDialogModel
{
    Q_OBJECT

    Q_PROPERTY(kn::notation::StyleItem * slurEndWidth READ slurEndWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * slurMidWidth READ slurMidWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * slurDottedWidth READ slurDottedWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * slurMinDistance READ slurMinDistance CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * angleHangingSlursAwayFromStaff READ angleHangingSlursAwayFromStaff CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * tieEndWidth READ tieEndWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * tieMidWidth READ tieMidWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * tieDottedWidth READ tieDottedWidth CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * tieMinDistance READ tieMinDistance CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * minTieLength READ minTieLength CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * minHangingTieLength READ minHangingTieLength CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * tiePlacementSingleNote READ tiePlacementSingleNote CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * tiePlacementChord READ tiePlacementChord CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * tieDotsPlacement READ tieDotsPlacement CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * minLaissezVibLength READ minLaissezVibLength CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * laissezVibUseSmuflSym READ laissezVibUseSmuflSym CONSTANT)

    QML_ELEMENT

public:
    explicit SlursAndTiesPageModel(QObject* parent = nullptr);

    StyleItem* slurEndWidth() const;
    StyleItem* slurMidWidth() const;
    StyleItem* slurDottedWidth() const;
    StyleItem* slurMinDistance() const;
    StyleItem* angleHangingSlursAwayFromStaff() const;
    StyleItem* tieEndWidth() const;
    StyleItem* tieMidWidth() const;
    StyleItem* tieDottedWidth() const;
    StyleItem* tieMinDistance() const;
    StyleItem* minTieLength() const;
    StyleItem* minHangingTieLength() const;
    StyleItem* tiePlacementSingleNote() const;
    StyleItem* tiePlacementChord() const;
    StyleItem* tieDotsPlacement() const;
    StyleItem* minLaissezVibLength() const;
    StyleItem* laissezVibUseSmuflSym() const;
};
}
