/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include "async/asyncable.h"
#include "context/iuicontextresolver.h"
#include "engraving/iengravingconfiguration.h"
#include "modularity/ioc.h"
#include "notation/notationtypes.h"
#include "ui/iuiactionsmodule.h"

#include "notationactioncontroller.h"

namespace kn::notation {
class NotationUiActions : public kfw::ui::IUiActionsModule, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<engraving::IEngravingConfiguration> engravingConfiguration;
    kfw::ContextInject<context::IUiContextResolver> uicontextResolver = { this };

public:

    NotationUiActions(std::shared_ptr<NotationActionController> controller, const kfw::modularity::ContextPtr& iocCtx);

    void init();

    const kfw::ui::UiActionList& actionsList() const override;

    bool actionEnabled(const kfw::ui::UiAction& act) const override;
    kfw::async::Channel<kfw::actions::ActionCodeList> actionEnabledChanged() const override;

    bool actionChecked(const kfw::ui::UiAction& act) const override;
    kfw::async::Channel<kfw::actions::ActionCodeList> actionCheckedChanged() const override;

    static DurationType actionDurationType(const kfw::actions::ActionCode& actionCode);
    static AccidentalType actionAccidentalType(const kfw::actions::ActionCode& actionCode);
    static int actionDotCount(const kfw::actions::ActionCode& actionCode);
    static int actionVoice(const kfw::actions::ActionCode& actionCode);
    static SymbolId actionArticulationSymbolId(const kfw::actions::ActionCode& actionCode);

    static const kfw::ui::ToolConfig& defaultNoteInputBarConfig();

private:
    static const kfw::ui::UiActionList s_actions;
    static const kfw::ui::UiActionList s_undoRedoActions;
    static const kfw::ui::UiActionList s_scoreConfigActions;
    static const kfw::ui::UiActionList s_engravingDebuggingActions;

    void updateActionsEnabled(const kfw::ui::UiActionList& actions);

    bool isScoreConfigAction(const kfw::actions::ActionCode& code) const;
    bool isScoreConfigChecked(const kfw::actions::ActionCode& code, const ScoreConfig& cfg) const;

    std::shared_ptr<NotationActionController> m_controller;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionEnabledChanged;
    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionCheckedChanged;

    std::unordered_map<kfw::actions::ActionCode, bool> m_actionEnabledMap;
};
}
