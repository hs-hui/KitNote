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

#include <QPaintEvent>
#include <QFrame>

#include "engraving/editing/scoreview.h"

#include "modularity/ioc.h"
#include "notation/inotationconfiguration.h"
#include "draw/types/transform.h"

namespace kn::engraving {
class EngravingItem;
class Score;
}

namespace kn::notation {
class ExampleView : public QFrame, public engraving::ScoreView, public kfw::Contextable
{
    Q_OBJECT

    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;

public:
    ExampleView(QWidget* parent = 0);
    ~ExampleView();
    void resetMatrix();
    void layoutChanged() override;
    void dataChanged(const kfw::RectF&) override;
    void updateAll() override;
    void adjustCanvasPosition(const engraving::EngravingItem* el, int staff = -1) override;
    void setScore(engraving::Score*) override;
    void removeScore() override;

    void changeEditElement(engraving::EngravingItem*) override;
    void setDropRectangle(const kfw::RectF&) override;
    void drawBackground(kfw::draw::Painter*, const kfw::RectF&) const override;
    void dragExampleView(QMouseEvent* ev);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

    kfw::PointF toLogical(const QPointF& point);

private:
    void drawElements(kfw::draw::Painter& painter, const std::vector<EngravingItem*>& el);
    void paintEvent(QPaintEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void constraintCanvas(int* dxx);
    QSize sizeHint() const override;

    kfw::draw::Transform m_matrix;
    QColor m_backgroundColor;
    QPixmap* m_backgroundPixmap;

    bool m_isDragging = false;
    kfw::PointF m_moveStartPoint;

    double m_defaultScaling = 0;
};
}
