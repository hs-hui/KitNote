/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include <QPainter>
#include "paintedengravingitem.h"

#include "notationscene/utilities/engravingitempreviewpainter.h"

using namespace kn::notation;

PaintedEngravingItem::PaintedEngravingItem(QQuickItem* parent)
    : QQuickPaintedItem(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

QVariant PaintedEngravingItem::engravingItemVariant() const
{
    return QVariant::fromValue(m_item);
}

void PaintedEngravingItem::setEngravingItemVariant(QVariant engravingItemVariant)
{
    kn::engraving::ElementPtr item = engravingItemVariant.value<kn::engraving::ElementPtr>();
    if (item == m_item) {
        return;
    }
    m_item = item;

    update();
    emit engravingItemVariantChanged();
}

int PaintedEngravingItem::numStaffLines() const
{
    return m_numStaffLines;
}

void PaintedEngravingItem::setNumStaffLines(int numStaffLines)
{
    if (m_numStaffLines == numStaffLines) {
        return;
    }
    m_numStaffLines = numStaffLines;

    update();
    emit numStaffLinesChanged();
}

double PaintedEngravingItem::spatium() const
{
    return m_spatium;
}

void PaintedEngravingItem::setSpatium(double spatium)
{
    if (spatium == m_spatium) {
        return;
    }
    m_spatium = spatium;

    update();
    emit spatiumChanged();
}

void PaintedEngravingItem::paint(QPainter* painter)
{
    kfw::draw::Painter p(painter, "paintedengravingitem");
    p.save();
    p.setAntialiasing(true);
    paintNotationPreview(p);
    p.restore();
}

void PaintedEngravingItem::paintNotationPreview(kfw::draw::Painter& painter) const
{
    IF_ASSERT_FAILED(m_item) {
        return;
    }

    EngravingItemPreviewPainter::PaintParams params;
    params.painter = &painter;
    params.color = configuration()->defaultColor();
    params.rect = kfw::RectF(0, 0, parentItem()->width(), parentItem()->height());
    params.spatium = m_spatium;
    params.numStaffLines = m_numStaffLines;

    EngravingItemPreviewPainter::paintPreview(renderer(), m_item.get(), params);
}
