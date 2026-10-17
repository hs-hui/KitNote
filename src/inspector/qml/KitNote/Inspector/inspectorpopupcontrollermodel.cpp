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

#include "inspectorpopupcontrollermodel.h"

#include "uicomponents/qml/KitNote/UiComponents/popupview.h"

using namespace kn::inspector;
using namespace kfw::uicomponents;

InspectorPopupControllerModel::InspectorPopupControllerModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void InspectorPopupControllerModel::classBegin()
{
    init();
}

void InspectorPopupControllerModel::init()
{
    popupController()->popupChanged().onNotify(this, [this]() {
        emit isAnyPopupOpenChanged();
    });

    popupController()->dropdownChanged().onNotify(this, [this]() {
        emit isAnyPopupOpenChanged();
    });

    popupController()->menuChanged().onNotify(this, [this]() {
        emit isAnyPopupOpenChanged();
    });
}

bool InspectorPopupControllerModel::isAnyPopupOpen() const
{
    auto controller = popupController();
    return controller->popup() || controller->dropdown() || controller->menu();
}

void InspectorPopupControllerModel::setNotationView(const QQuickItem* view)
{
    popupController()->setNotationView(view);
}

void InspectorPopupControllerModel::setPopup(PopupView* popup, QQuickItem* control)
{
    popupController()->setPopup(popup, control);
}

void InspectorPopupControllerModel::setDropdown(DropdownView* dropdown)
{
    popupController()->setDropdown(dropdown);
}

void InspectorPopupControllerModel::setMenu(MenuView* menu)
{
    popupController()->setMenu(menu);
}

void InspectorPopupControllerModel::repositionPopupIfNeed()
{
    if (popupController()->popup()) {
        popupController()->popup()->repositionWindowIfNeed();
    }
}
