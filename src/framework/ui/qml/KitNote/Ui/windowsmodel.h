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

#include <qqmlintegration.h>

#include "modularity/ioc.h"
#include "ui/iwindowscontroller.h"

namespace kfw::ui {
class WindowsModel : public QObject, public Contextable
{
    Q_OBJECT
    QML_ELEMENT;

    ContextInject<ui::IWindowsController> windowsController = { this };

    Q_PROPERTY(
        QRect mainWindowTitleBarMoveArea READ mainWindowTitleBarMoveArea WRITE setMainWindowTitleBarMoveArea NOTIFY mainWindowTitleBarMoveAreaChanged)

public:
    explicit WindowsModel(QObject* parent = nullptr);

    QRect mainWindowTitleBarMoveArea() const;

public slots:
    void setMainWindowTitleBarMoveArea(const QRect& area);

signals:
    void mainWindowTitleBarMoveAreaChanged(QRect titleBarMoveArea);
};
}
