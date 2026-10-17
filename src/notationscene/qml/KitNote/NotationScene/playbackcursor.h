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

#include "modularity/ioc.h"
#include "notation/inotationconfiguration.h"
#include "draw/types/geometry.h"
#include "midi/miditypes.h"

#include "notation/inotation.h"

class QColor;

namespace kn::notation {
class PlaybackCursor : public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> configuration;

public:
    PlaybackCursor(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void paint(kfw::draw::Painter* painter);

    void setNotation(INotationPtr notation);
    void move(kfw::midi::tick_t tick);

    bool visible() const;
    void setVisible(bool arg);

    const kfw::RectF& rect() const;

private:
    QColor color() const;
    kfw::RectF resolveCursorRectByTick(kfw::midi::tick_t tick) const;

    bool m_visible = false;
    kfw::RectF m_rect;

    INotationPtr m_notation;
};
}
