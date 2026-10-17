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
#ifndef KN_APPSHELL_NOTATIONPAGEMODEL_H
#define KN_APPSHELL_NOTATIONPAGEMODEL_H

#include <QQuickItem>

#include "modularity/ioc.h"
#include "async/asyncable.h"
#include "actions/actionable.h"
#include "actions/iactionsdispatcher.h"
#include "context/iglobalcontext.h"
#include "iappshellconfiguration.h"
#include "notation/inotationconfiguration.h"
#include "braille/ibrailleconfiguration.h"
#include "dockwindow/idockwindowprovider.h"

namespace kn::appshell {
class NotationPageModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable, public kfw::actions::Actionable
{
    Q_OBJECT

    Q_PROPERTY(bool isNavigatorVisible READ isNavigatorVisible NOTIFY isNavigatorVisibleChanged)
    Q_PROPERTY(bool isBraillePanelVisible READ isBraillePanelVisible NOTIFY isBraillePanelVisibleChanged)

    kfw::GlobalInject<IAppShellConfiguration> configuration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<braille::IBrailleConfiguration> brailleConfiguration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::dock::IDockWindowProvider> dockWindowProvider = { this };

public:
    explicit NotationPageModel(QObject* parent = nullptr);

    bool isNavigatorVisible() const;
    bool isBraillePanelVisible() const;

    Q_INVOKABLE void init();

    Q_INVOKABLE QString notationToolBarName() const;
    Q_INVOKABLE QString playbackToolBarName() const;
    Q_INVOKABLE QString undoRedoToolBarName() const;
    Q_INVOKABLE QString noteInputBarName() const;

    Q_INVOKABLE QString palettesPanelName() const;
    Q_INVOKABLE QString layoutPanelName() const;
    Q_INVOKABLE QString inspectorPanelName() const;
    Q_INVOKABLE QString selectionFiltersPanelName() const;
    Q_INVOKABLE QString undoHistoryPanelName() const;

    Q_INVOKABLE QString mixerPanelName() const;
    Q_INVOKABLE QString pianoKeyboardPanelName() const;
    Q_INVOKABLE QString timelinePanelName() const;
    Q_INVOKABLE QString drumsetPanelName() const;
    Q_INVOKABLE QString percussionPanelName() const;

    Q_INVOKABLE QString statusBarName() const;

signals:
    void isNavigatorVisibleChanged();
    void isBraillePanelVisibleChanged();

private:
    void onNotationChanged();

    void toggleDock(const QString& name);

    void updateDrumsetPanelVisibility();
    void updatePercussionPanelVisibility();
};
}

#endif // KN_APPSHELL_NOTATIONPAGEMODEL_H
