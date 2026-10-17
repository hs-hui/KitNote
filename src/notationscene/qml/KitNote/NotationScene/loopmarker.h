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

#include "notation/inotationconfiguration.h"
#include "modularity/ioc.h"

#include "notation/inotation.h"

#include "draw/types/geometry.h"

namespace kn::notation {
class LoopMarker : public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> configuration;

public:
    LoopMarker(LoopBoundaryType type, const kfw::modularity::ContextPtr& iocCtx);

    void setNotation(INotationPtr notation);
    void setVisible(bool visible);

    void updatePosition(engraving::Fraction tick);

    void paint(kfw::draw::Painter* painter);

private:
    kfw::RectF resolveMarkerRectByTick(engraving::Fraction tick) const;

    LoopBoundaryType m_type = LoopBoundaryType::Unknown;
    kfw::RectF m_rect;
    bool m_visible = false;
    INotationPtr m_notation;
};
}
