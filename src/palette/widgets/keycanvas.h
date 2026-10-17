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

#ifndef __KEYCANVAS_H__
#define __KEYCANVAS_H__

#include <QFrame>

#include "modularity/ioc.h"
#include "ipaletteconfiguration.h"
#include "notation/inotationconfiguration.h"
#include "engraving/iengravingconfiguration.h"
#include "engraving/rendering/isinglerenderer.h"

namespace kn::engraving {
class Accidental;
class Clef;
}

namespace kn::palette {
//---------------------------------------------------------
//   KeyCanvas
//---------------------------------------------------------

class KeyCanvas : public QFrame, public kfw::Contextable
{
    Q_OBJECT

    kfw::GlobalInject<IPaletteConfiguration> configuration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<engraving::IEngravingConfiguration> engravingConfiguration;
    kfw::ContextInject<engraving::rendering::ISingleRenderer> engravingRender = { this };

    engraving::Accidental* dragElement = nullptr;
    engraving::Accidental* moveElement = nullptr;
    QTransform _matrix, imatrix;
    double extraMag = 1.0;
    QList<engraving::Accidental*> accidentals;
    QPointF startMove;
    QPointF base;
    engraving::Clef* clef = nullptr;

    virtual void paintEvent(QPaintEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);

    virtual void dragEnterEvent(QDragEnterEvent*);
    virtual void dragMoveEvent(QDragMoveEvent*);
    virtual void dropEvent(QDropEvent*);
    void snap(engraving::Accidental*);

private slots:
    void deleteElement();

public:
    KeyCanvas(QWidget* parent = 0);
    void clear();
    const QList<engraving::Accidental*> getAccidentals() const { return accidentals; }
};
} // namespace kn::palette
#endif
