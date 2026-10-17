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

#ifndef KN_PALETTE_PALETTEACTIONSCONTROLLER_H
#define KN_PALETTE_PALETTEACTIONSCONTROLLER_H

#include "modularity/ioc.h"
#include "actions/actionable.h"
#include "actions/iactionsdispatcher.h"
#include "ui/iuiactionsregister.h"
#include "async/asyncable.h"
#include "iinteractive.h"
#include "context/iglobalcontext.h"

namespace kn::palette {
class PaletteActionsController : public kfw::actions::Actionable, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    PaletteActionsController(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    void init();

    kfw::ValCh<bool> isMasterPaletteOpened() const;

private:
    void toggleMasterPalette(const kfw::actions::ActionData& args);
    void toggleSpecialCharactersDialog();
    void openTimeSignaturePropertiesDialog();
    void openCustomizeKitDialog();

    notation::INotationInteractionPtr interaction() const;

    kfw::ValCh<bool> m_masterPaletteOpened;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionsReceiveAvailableChanged;
};
}

#endif // KN_PALETTE_PALETTEACTIONSCONTROLLER_H
