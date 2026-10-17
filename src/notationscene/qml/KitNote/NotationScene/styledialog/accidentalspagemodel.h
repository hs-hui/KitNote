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
class AccidentalsPageModel : public AbstractStyleDialogModel
{
    Q_OBJECT

    Q_PROPERTY(kn::notation::StyleItem * bracketedAccidentalPadding READ bracketedAccidentalPadding CONSTANT)

    Q_PROPERTY(kn::notation::StyleItem * accidFollowNoteOffset READ accidFollowNoteOffset CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * alignAccidentalOctavesAcrossSubChords READ alignAccidentalOctavesAcrossSubChords CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * keepAccidentalSecondsTogether READ keepAccidentalSecondsTogether CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * alignOffsetOctaveAccidentals READ alignOffsetOctaveAccidentals CONSTANT)

    QML_ELEMENT

public:
    explicit AccidentalsPageModel(QObject* parent = nullptr);

    StyleItem* bracketedAccidentalPadding() const;
    StyleItem* accidFollowNoteOffset() const;
    StyleItem* alignAccidentalOctavesAcrossSubChords() const;
    StyleItem* keepAccidentalSecondsTogether() const;
    StyleItem* alignOffsetOctaveAccidentals() const;
};
}
