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
class MeasureRepeatModel : public AbstractStyleDialogModel
{
    Q_OBJECT

    Q_PROPERTY(kn::notation::StyleItem * measureRepeatNumberPos READ measureRepeatNumberPos CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * mrNumberSeries READ mrNumberSeries CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * oneMeasureRepeatShow1 READ oneMeasureRepeatShow1 CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * fourMeasureRepeatShowExtenders READ fourMeasureRepeatShowExtenders CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * mrNumberEveryXMeasures READ mrNumberEveryXMeasures CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * mrNumberSeriesWithParentheses READ mrNumberSeriesWithParentheses CONSTANT)

    QML_ELEMENT

public:
    explicit MeasureRepeatModel(QObject* parent = nullptr);

    StyleItem* measureRepeatNumberPos() const;
    StyleItem* mrNumberSeries() const;
    StyleItem* oneMeasureRepeatShow1() const;
    StyleItem* fourMeasureRepeatShowExtenders() const;
    StyleItem* mrNumberEveryXMeasures() const;
    StyleItem* mrNumberSeriesWithParentheses() const;
};
}
