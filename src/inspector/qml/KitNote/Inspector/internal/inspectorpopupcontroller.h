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

#include "iinspectorpopupcontroller.h"
#include "ui/imainwindow.h"
#include "global/iinteractive.h"

namespace kfw::uicomponents {
class PopupView;
}

namespace kn::inspector {
class InspectorPopupController : public QObject, public IInspectorPopupController, public kfw::Contextable
{
    Q_OBJECT

    kfw::ContextInject<kfw::ui::IMainWindow> mainWindow = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:
    explicit InspectorPopupController(const kfw::modularity::ContextPtr& iocCtx);
    ~InspectorPopupController() override;

    void init();

    void setNotationView(const QQuickItem* view) override;

    kfw::uicomponents::PopupView* popup() const override;
    void setPopup(kfw::uicomponents::PopupView* popup, QQuickItem* control = nullptr) override;
    kfw::async::Notification popupChanged() const override;

    kfw::uicomponents::DropdownView* dropdown() const override;
    void setDropdown(kfw::uicomponents::DropdownView* dropdown) override;
    kfw::async::Notification dropdownChanged() const override;

    kfw::uicomponents::MenuView* menu() const override;
    void setMenu(kfw::uicomponents::MenuView* menu) override;
    kfw::async::Notification menuChanged() const override;

private slots:
    void closePopup();
    void doClosePopup();

private:
    bool eventFilter(QObject* watched, QEvent* event) override;

    void closePopupIfNeed(const QPointF& mouseGlobalPos);

    const QQuickItem* m_notationView = nullptr;

    kfw::uicomponents::PopupView* m_popup = nullptr;
    QQuickItem* m_visualControl = nullptr;
    kfw::async::Notification m_popupChanged;

    kfw::uicomponents::DropdownView* m_dropdown = nullptr;
    kfw::async::Notification m_dropdownChanged;

    kfw::uicomponents::MenuView* m_menu = nullptr;
    kfw::async::Notification m_menuChanged;
};
}
