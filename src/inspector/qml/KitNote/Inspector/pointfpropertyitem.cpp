/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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
#include "pointfpropertyitem.h"

#include "dataformatter.h"

using namespace kn::inspector;

PointFPropertyItem::PointFPropertyItem(const kn::engraving::Pid propertyId, QObject* parent)
    : kn::inspector::PropertyItem(propertyId, parent)
{
}

qreal PointFPropertyItem::x_property() const
{
    return kfw::DataFormatter::roundDouble(value().toPointF().x());
}

qreal PointFPropertyItem::x() const
{
    return value().toPointF().x();
}

void PointFPropertyItem::setX(qreal newX)
{
    setValue(QPointF(newX, y()));
}

qreal PointFPropertyItem::y_property() const
{
    return kfw::DataFormatter::roundDouble(value().toPointF().y());
}

qreal PointFPropertyItem::y() const
{
    return value().toPointF().y();
}

void PointFPropertyItem::setY(qreal newY)
{
    setValue(QPointF(x(), newY));
}
