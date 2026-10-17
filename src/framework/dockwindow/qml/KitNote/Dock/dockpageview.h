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

#include <QQuickItem>
#include <qqmlintegration.h>

#include "uicomponents/qml/KitNote/UiComponents/qmllistproperty.h"

#include "modularity/ioc.h"
#include "ui/inavigationcontroller.h"
#include "tours/itoursservice.h"

#include "internal/dockbase.h"

#include "dockstatusbar.h"
#include "docktypes.h"

namespace kfw::ui {
class NavigationControl;
}

namespace kfw::dock {
class DockToolBarView;
class DockPanelView;
class DockCentralView;
class DockStatusBar;
class DockingHolderView;
class DockPageView : public QQuickItem, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(QString uri READ uri WRITE setUri NOTIFY uriChanged)
    Q_PROPERTY(QQmlListProperty<kfw::dock::DockToolBarView> mainToolBars READ mainToolBarsProperty CONSTANT)
    Q_PROPERTY(QQmlListProperty<kfw::dock::DockToolBarView> toolBars READ toolBarsProperty CONSTANT)
    Q_PROPERTY(QQmlListProperty<kfw::dock::DockingHolderView> toolBarsDockingHolders READ toolBarsDockingHoldersProperty CONSTANT)
    Q_PROPERTY(QQmlListProperty<kfw::dock::DockPanelView> panels READ panelsProperty CONSTANT)
    Q_PROPERTY(QQmlListProperty<kfw::dock::DockingHolderView> panelsDockingHolders READ panelsDockingHoldersProperty)
    Q_PROPERTY(kfw::dock::DockCentralView * centralDock READ centralDock WRITE setCentralDock NOTIFY centralDockChanged)
    Q_PROPERTY(kfw::dock::DockStatusBar * statusBar READ statusBar WRITE setStatusBar NOTIFY statusBarChanged)

    Q_PROPERTY(QVariant tours READ tours WRITE setTours NOTIFY toursChanged)

    QML_ELEMENT

    ContextInject<ui::INavigationController> navigationController = { this };
    ContextInject<tours::IToursService> toursService = { this };

public:
    explicit DockPageView(QQuickItem* parent = nullptr);

    void init();
    void deinit();

    QString uri() const;

    void setParams(const QVariantMap& params);

    QQmlListProperty<DockToolBarView> mainToolBarsProperty();
    QQmlListProperty<DockToolBarView> toolBarsProperty();
    QQmlListProperty<DockingHolderView> toolBarsDockingHoldersProperty();
    QQmlListProperty<DockPanelView> panelsProperty();
    QQmlListProperty<DockingHolderView> panelsDockingHoldersProperty();

    QList<DockToolBarView*> mainToolBars() const;
    QList<DockToolBarView*> toolBars() const;
    QList<DockingHolderView*> toolBarsHolders() const;
    DockCentralView* centralDock() const;
    DockStatusBar* statusBar() const;
    QList<DockPanelView*> panels() const;
    QList<DockingHolderView*> panelsHolders() const;
    QList<DockBase*> allDocks() const;

    DockBase* dockByName(const QString& dockName) const;
    DockingHolderView* holder(DockType type, Location location) const;

    QList<DockPanelView*> findPanelsForDropping(const DockPanelView* panel) const;
    DockPanelView* findPanelForTab(const DockPanelView* tab) const;

    bool isDockOpen(const QString& dockName) const;
    void toggleDock(const QString& dockName);
    void setDockOpen(const QString& dockName, bool open);

    bool isDockFloating(const QString& dockName) const;
    void toggleDockFloating(const QString& dockName);

    Q_INVOKABLE void setDefaultNavigationControl(kfw::ui::NavigationControl* control);

    Q_INVOKABLE void forceLayout();

    QVariant tours() const;
    void setTours(const QVariant& newTours);

public slots:
    void setUri(const QString& uri);
    void setCentralDock(DockCentralView* central);
    void setStatusBar(DockStatusBar* statusBar);

signals:
    void inited();
    void setParamsRequested(const QVariantMap& params);
    void uriChanged(const QString& uri);
    void centralDockChanged(DockCentralView* central);
    void statusBarChanged(DockStatusBar* statusBar);

    void toursChanged();
    void layoutRequested();

private:
    void componentComplete() override;

    void reorderSections();
    void doReorderSections();
    void reorderDocksNavigationSections(QList<DockBase*>& docks);
    void reorderNavigationSectionPanels(QList<DockBase*>& sectionDocks);

    QString m_uri;
    uicomponents::QmlListProperty<DockToolBarView> m_mainToolBars;
    uicomponents::QmlListProperty<DockToolBarView> m_toolBars;
    uicomponents::QmlListProperty<DockingHolderView> m_toolBarsDockingHolders;
    uicomponents::QmlListProperty<DockPanelView> m_panels;
    uicomponents::QmlListProperty<DockingHolderView> m_panelsDockingHolders;
    DockCentralView* m_central = nullptr;
    DockStatusBar* m_statusBar = nullptr;

    QVariant m_tours;
};
}
