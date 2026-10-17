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

#include <QObject>
#include <QMouseEvent>
#include <QPainter>
#include <QQuickPaintedItem>
#include <qqmlintegration.h>

#include "draw/types/geometry.h"
#include "modularity/ioc.h"
#include "notation/inotationconfiguration.h"
#include "context/iglobalcontext.h"
#include "ui/iuiconfiguration.h"
#include "engraving/iengravingconfiguration.h"
#include "abstractnotationpaintview.h"

namespace kn::notation {
class NotationNavigatorCursorView : public QQuickPaintedItem, public kfw::Contextable
{
    Q_OBJECT

    kfw::GlobalInject<INotationConfiguration> configuration;

public:
    NotationNavigatorCursorView(QQuickItem* parent = nullptr);

    void setRect(const kfw::RectF& cursorRect);

private:
    virtual void paint(QPainter* painter) override;

    kfw::RectF m_cursorRect;
};

class NotationNavigator : public AbstractNotationPaintView
{
    Q_OBJECT

    Q_PROPERTY(int orientation READ orientation NOTIFY orientationChanged)

    QML_ELEMENT;

    kfw::GlobalInject<INotationConfiguration> configuration;
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::GlobalInject<engraving::IEngravingConfiguration> engravingConfiguration;
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    NotationNavigator(QQuickItem* parent = nullptr);

    Q_INVOKABLE void load();
    Q_INVOKABLE void setCursorRect(const QRectF& rect);

    int orientation() const;

signals:
    void moveNotationRequested(qreal dx, qreal dy);
    void orientationChanged();

private:
    void onLoadNotation(INotationPtr notation) override;

    void initOrientation();
    void initVisible();

    ViewMode notationViewMode() const;

    void rescaleIfVisible();

    void paint(QPainter* painter) override;
    void onViewSizeChanged() override;

    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    void paintPageNumbers(QPainter* painter);

    bool moveCanvasToRect(const kfw::RectF& viewRect);

    bool isVerticalOrientation() const;

    const PageList& pages() const;

    kfw::RectF m_cursorRect;
    NotationNavigatorCursorView* m_cursorRectView = nullptr;
    kfw::PointF m_startMove;
};
}
