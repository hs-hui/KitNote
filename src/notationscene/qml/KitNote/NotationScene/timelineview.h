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

#include <QImage>
#include <QTimer>
#include <qqmlintegration.h>

#include "uicomponents/qml/KitNote/UiComponents/widgetview.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "async/asyncable.h"

namespace kn::notation {
class TimelineView : public kfw::uicomponents::WidgetView, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT
    QML_ELEMENT;

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    explicit TimelineView(QQuickItem* parent = nullptr);

private slots:
    void doDraw();

private:

    void paint(QPainter* painter) override;
    void componentComplete() override;

    qreal m_dpr = 1.0; // device pixel ratio
    QImage m_image;
    QTimer m_drawTimer;
};
}
