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

#include "ui/iuiactionsmodule.h"
#include "applicationactioncontroller.h"
#include "modularity/ioc.h"
#include "async/asyncable.h"
#include "ui/imainwindow.h"
#include "braille/ibrailleconfiguration.h"
#include "notation/inotationconfiguration.h"
#include "notationscene/inotationsceneconfiguration.h"

#include "dockwindow/idockwindowprovider.h"

namespace kn::appshell {
class ApplicationUiActions : public kfw::ui::IUiActionsModule, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<IAppShellConfiguration> configuration;
    kfw::GlobalInject<braille::IBrailleConfiguration> brailleConfiguration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<notation::INotationSceneConfiguration> notationSceneConfiguration;
    kfw::ContextInject<kfw::ui::IMainWindow> mainWindow = { this };
    kfw::ContextInject<kfw::dock::IDockWindowProvider> dockWindowProvider = { this };

public:
    ApplicationUiActions(std::shared_ptr<ApplicationActionController> controller, const kfw::modularity::ContextPtr& iocCtx);

    void init();

    const kfw::ui::UiActionList& actionsList() const override;

    bool actionEnabled(const kfw::ui::UiAction& act) const override;
    kfw::async::Channel<kfw::actions::ActionCodeList> actionEnabledChanged() const override;

    bool actionChecked(const kfw::ui::UiAction& act) const override;
    kfw::async::Channel<kfw::actions::ActionCodeList> actionCheckedChanged() const override;

    static const QMap<kfw::actions::ActionCode, DockName>& toggleDockActions();

private:
    void listenOpenedDocksChanged(kfw::dock::IDockWindow* window);

    static const kfw::ui::UiActionList m_actions;

    std::shared_ptr<ApplicationActionController> m_controller;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionEnabledChanged;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionCheckedChanged;
};
}
