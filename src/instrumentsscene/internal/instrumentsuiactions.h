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
#ifndef KN_INSTRUMENTSSCENE_INSTRUMENTSUIACTIONS_H
#define KN_INSTRUMENTSSCENE_INSTRUMENTSUIACTIONS_H

#include "ui/iuiactionsmodule.h"
#include "modularity/ioc.h"
#include "context/iuicontextresolver.h"

namespace kn::instrumentsscene {
class InstrumentsUiActions : public kfw::ui::IUiActionsModule, public kfw::Contextable
{
    kfw::ContextInject<context::IUiContextResolver> uicontextResolver = { this };
public:

    InstrumentsUiActions(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {}

    const kfw::ui::UiActionList& actionsList() const override;

    bool actionEnabled(const kfw::ui::UiAction& act) const override;
    kfw::async::Channel<kfw::actions::ActionCodeList> actionEnabledChanged() const override;

    bool actionChecked(const kfw::ui::UiAction& act) const override;
    kfw::async::Channel<kfw::actions::ActionCodeList> actionCheckedChanged() const override;

private:
    const static kfw::ui::UiActionList m_actions;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionEnabledChanged;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionCheckedChanged;
};
}

#endif // KN_INSTRUMENTSSCENE_INSTRUMENTSUIACTIONS_H
