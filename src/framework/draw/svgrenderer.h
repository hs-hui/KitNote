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
#ifndef KFW_DRAW_SVGRENDERER_H
#define KFW_DRAW_SVGRENDERER_H

#include "global/types/bytearray.h"

#include "types/geometry.h"
#include "painter.h"

class QSvgRenderer;

namespace kfw::draw {
class SvgRenderer
{
public:
    SvgRenderer(const ByteArray& data);
    ~SvgRenderer();

    SizeF defaultSize() const;

    void render(Painter* painter, const RectF& rect);

private:
    QSvgRenderer* m_qSvgRenderer = nullptr;
};
}

#endif // KFW_DRAW_SVGRENDERER_H
