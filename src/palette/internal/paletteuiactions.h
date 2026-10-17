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
#ifndef KN_PALETTE_PALETTEUIACTIONS_H
#define KN_PALETTE_PALETTEUIACTIONS_H

#include "ui/iuiactionsmodule.h"
#include "paletteactionscontroller.h"
#include "modularity/ioc.h"
#include "context/iuicontextresolver.h"
#include "async/asyncable.h"

namespace kn::palette {
class PaletteUiActions : public kfw::ui::IUiActionsModule, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::ContextInject<context::IUiContextResolver> uicontextResolver = { this };

public:
    PaletteUiActions(std::shared_ptr<PaletteActionsController> controller, const kfw::modularity::ContextPtr& iocCtx);

    void init();

    const kfw::ui::UiActionList& actionsList() const override;

    bool actionEnabled(const kfw::ui::UiAction& act) const override;
    kfw::async::Channel<kfw::actions::ActionCodeList> actionEnabledChanged() const override;

    bool actionChecked(const kfw::ui::UiAction& act) const override;
    kfw::async::Channel<kfw::actions::ActionCodeList> actionCheckedChanged() const override;

private:
    const static kfw::ui::UiActionList m_actions;
    std::shared_ptr<PaletteActionsController> m_controller;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionEnabledChanged;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionCheckedChanged;
};
}

#endif // KN_PALETTE_PALETTEUIACTIONS_H
