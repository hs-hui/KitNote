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
#include <QQmlParserStatus>

#include "async/asyncable.h"
#include "actions/actionable.h"

#include "uicomponents/qml/KitNote/UiComponents/menuitem.h"
#include "uicomponents/qml/KitNote/UiComponents/abstractmenumodel.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"
#include "ui/iuiactionsregister.h"
#include "workspace/iworkspaceconfiguration.h"
#include "notation/inotationconfiguration.h"
#include "context/iglobalcontext.h"

#include "notation/notationtypes.h"

#include "global/iglobalconfiguration.h"

namespace kn::appshell {
class NotationStatusBarModel : public QObject, public QQmlParserStatus, public kfw::Contextable, public kfw::async::Asyncable,
    public kfw::actions::Actionable
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QString accessibilityInfo READ accessibilityInfo NOTIFY accessibilityInfoChanged)
    Q_PROPERTY(kfw::uicomponents::MenuItem * currentWorkspaceItem READ currentWorkspaceItem CONSTANT)
    Q_PROPERTY(kfw::uicomponents::MenuItem * concertPitchItem READ concertPitchItem CONSTANT)
    Q_PROPERTY(kfw::uicomponents::MenuItem * currentViewMode READ currentViewMode NOTIFY currentViewModeChanged)
    Q_PROPERTY(bool zoomEnabled READ zoomEnabled NOTIFY zoomEnabledChanged)
    Q_PROPERTY(QVariantList availableViewModeList READ availableViewModeList_property NOTIFY availableViewModeListChanged)
    Q_PROPERTY(QVariantList availableZoomList READ availableZoomList_property NOTIFY availableZoomListChanged)
    Q_PROPERTY(int currentZoomPercentage READ currentZoomPercentage WRITE setCurrentZoomPercentage NOTIFY currentZoomPercentageChanged)

    kfw::GlobalInject<kfw::workspace::IWorkspaceConfiguration> workspaceConfiguration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::ContextInject<context::IGlobalContext> context = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<kfw::ui::IUiActionsRegister> actionsRegister = { this };

public:
    explicit NotationStatusBarModel(QObject* parent = nullptr);

    QString accessibilityInfo() const;
    kfw::uicomponents::MenuItem* currentWorkspaceItem();
    kfw::uicomponents::MenuItem* concertPitchItem();
    kfw::uicomponents::MenuItem* currentViewMode();
    bool zoomEnabled() const;
    int currentZoomPercentage() const;

    Q_INVOKABLE void toggleConcertPitch();
    Q_INVOKABLE void setCurrentViewMode(const QString& modeCode);

    Q_INVOKABLE int minZoomPercentage() const;
    Q_INVOKABLE int maxZoomPercentage() const;
    Q_INVOKABLE void setCurrentZoom(const QString& zoomId);
    Q_INVOKABLE void zoomIn();
    Q_INVOKABLE void zoomOut();

    Q_INVOKABLE void handleAction(const QString& actionCode);
    Q_INVOKABLE void handleWorkspacesMenuItem(const QString& itemId);

public slots:
    void setCurrentZoomPercentage(int zoomPercentage);

signals:
    void accessibilityInfoChanged();
    void currentWorkspaceActionChanged();
    void concertPitchActionChanged();
    void currentViewModeChanged();
    void availableViewModeListChanged();
    void zoomEnabledChanged();
    void availableZoomListChanged();
    void currentZoomPercentageChanged();

private:
    void classBegin() override;
    void componentComplete() override {}
    void init();

    notation::INotationPtr notation() const;
    notation::INotationAccessibilityPtr accessibility() const;

    void initAvailableViewModeList();
    void initAvailableZoomList();

    kfw::uicomponents::MenuItem* makeMenuItem(const kfw::actions::ActionCode& actionCode);

    void updateConcertPitchItem();
    void updateCurrentWorkspaceItem();

    void dispatch(const kfw::actions::ActionCode& code, const kfw::actions::ActionData& args = kfw::actions::ActionData());

    void onCurrentNotationChanged();

    notation::ZoomType currentZoomType() const;

    void listenChangesInAccessibility();

    QList<int> possibleZoomPercentageList() const;

    QVariantList availableViewModeList_property();
    QVariantList availableZoomList_property();

    QVariantList menuItemListToVariantList(const kfw::uicomponents::MenuItemList& list) const;

    kfw::uicomponents::MenuItem* m_currentWorkspaceItem = nullptr;
    kfw::uicomponents::MenuItem* m_concertPitchItem = nullptr;
    kfw::uicomponents::MenuItemList m_availableViewModeList;
    kfw::uicomponents::MenuItemList m_availableZoomList;

    std::shared_ptr<kfw::uicomponents::AbstractMenuModel> m_workspacesMenuModel;
};
}
