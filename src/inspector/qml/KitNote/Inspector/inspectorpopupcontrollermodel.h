/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2026 MuseScore Limited and others
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
#include <QQmlParserStatus>

#include "modularity/ioc.h"
#include "internal/iinspectorpopupcontroller.h"

#include "async/asyncable.h"

Q_MOC_INCLUDE(< QQuickItem >)
Q_MOC_INCLUDE("uicomponents/qml/KitNote/UiComponents/popupview.h")
Q_MOC_INCLUDE("uicomponents/qml/KitNote/UiComponents/dropdownview.h")
Q_MOC_INCLUDE("uicomponents/qml/KitNote/UiComponents/menuview.h")

class QQuickItem;

namespace kn::inspector {
class InspectorPopupControllerModel : public QObject, public QQmlParserStatus, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    QML_ELEMENT;

    Q_PROPERTY(bool isAnyPopupOpen READ isAnyPopupOpen NOTIFY isAnyPopupOpenChanged)

    kfw::ContextInject<IInspectorPopupController> popupController  = { this };

public:
    explicit InspectorPopupControllerModel(QObject* parent = nullptr);

    bool isAnyPopupOpen() const;

    Q_INVOKABLE void setNotationView(const QQuickItem* view);
    Q_INVOKABLE void setPopup(kfw::uicomponents::PopupView* popup, QQuickItem* control = nullptr);
    Q_INVOKABLE void setDropdown(kfw::uicomponents::DropdownView* dropdown);
    Q_INVOKABLE void setMenu(kfw::uicomponents::MenuView* menu);
    Q_INVOKABLE void repositionPopupIfNeed();

signals:
    void isAnyPopupOpenChanged();

private:
    void classBegin() override;
    void componentComplete() override {}
    void init();
};
}
