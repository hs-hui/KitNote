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

#ifndef KFW_DOCK_DOCKTABBAR_H
#define KFW_DOCK_DOCKTABBAR_H

#include <QQuickItem>

#include "thirdparty/KDDockWidgets/src/private/quick/TabBarQuick_p.h"

namespace kfw::dock {
class DockTabBar : public KDDockWidgets::TabBarQuick
{
    Q_OBJECT

public:
    explicit DockTabBar(KDDockWidgets::TabWidget* parent = nullptr);

    // The following is a hack (see PR #29794)- revisit this system when updating KDDockWidgets
    Q_PROPERTY(bool tabChangedOnClick READ tabChangedOnClick CONSTANT)

    Q_INVOKABLE void setDraggableMouseArea(QQuickItem* mouseArea);
    Q_INVOKABLE void doubleClicked(const QPoint& pos) const;

    bool tabChangedOnClick() const { return m_tabChangedOnClick; }

private:
    bool event(QEvent* event) override;
    void onMousePressRelease(const QMouseEvent* mouseEvent);
    bool isPositionDraggable(QPoint localPos) const override;

    QQuickItem* m_draggableMouseArea = nullptr;
    int m_indexOfPressedTab = -1;

    bool m_tabChangedOnClick = false;
};
}

#endif // KFW_DOCK_DOCKTABBAR_H
