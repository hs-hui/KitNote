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

#include "notationscene/widgets/exampleview.h"

#include "modularity/ioc.h"
#include "engraving/rendering/isinglerenderer.h"

namespace kn::engraving {
class Note;
class Chord;
class ActionIcon;
}

namespace kn::palette {
class NoteGroupsExampleView : public notation::ExampleView
{
    Q_OBJECT

    kfw::ContextInject<engraving::rendering::ISingleRenderer> engravingRender = { this };

public:
    NoteGroupsExampleView(QWidget* parent = 0);

signals:
    void noteClicked(engraving::Note*);
    void beamPropertyDropped(engraving::Chord*, engraving::ActionIcon*);

private:
    void setDropTarget(engraving::EngravingItem* el) override;

    void dragEnterEvent(QDragEnterEvent*) override;
    void dragLeaveEvent(QDragLeaveEvent*) override;
    void dragMoveEvent(QDragMoveEvent*) override;
    void dropEvent(QDropEvent*) override;
    void mousePressEvent(QMouseEvent*) override;

    engraving::EngravingItem* m_dragElement = 0;
    const engraving::EngravingItem* m_dropTarget = 0; ///< current drop target during dragMove
    QLineF m_dropAnchor;                   ///< line to current anchor point during dragMove
};
}
