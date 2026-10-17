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

#include <QIconEngine>

#include "palettecell.h"

#include "modularity/ioc.h"
#include "ipaletteconfiguration.h"
#include "engraving/rendering/isinglerenderer.h"

namespace kfw::draw {
class Painter;
}

namespace kn::palette {
class PaletteCellIconEngine : public QIconEngine, public kfw::Contextable
{
    kfw::GlobalInject<IPaletteConfiguration> configuration;
    kfw::ContextInject<engraving::rendering::ISingleRenderer> engravingRender = { this };

public:
    explicit PaletteCellIconEngine(PaletteCellConstPtr cell, const kfw::modularity::ContextPtr& ctx, qreal extraMag = 1.0);

    QIconEngine* clone() const override;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;

private:
    void paintCell(kfw::draw::Painter& painter, const kfw::RectF& rect, bool selected, bool current) const;
    void paintBackground(kfw::draw::Painter& painter, const kfw::RectF& rect, bool selected, bool current) const;

    PaletteCellConstPtr m_cell;
    qreal m_extraMag = 1.0;
};
}
