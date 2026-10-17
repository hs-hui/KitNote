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

#ifndef KFW_UI_IUIENGINE_H
#define KFW_UI_IUIENGINE_H

#include <QString>

#include "modularity/imoduleinterface.h"

#include "uitypes.h"

class QQmlEngine;
class QQmlApplicationEngine;
class QQuickItem;

namespace kfw::ui {
class IUiEngine : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IUiEngine)

public:
    virtual ~IUiEngine() = default;

    virtual void updateTheme() = 0;
    virtual QQmlApplicationEngine* qmlAppEngine() const = 0;
    virtual QQmlEngine* qmlEngine() const = 0;
    virtual void quit() = 0;
    virtual void clearComponentCache() = 0;

    virtual GraphicsApi graphicsApi() const = 0;
    virtual QString graphicsApiName() const = 0;

    virtual void addSourceImportPath(const QString& path) = 0;
};
}

#endif // KFW_UI_UIENGINEMODULE_H
