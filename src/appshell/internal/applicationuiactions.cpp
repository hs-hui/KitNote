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
#include "applicationuiactions.h"

#include "ui/view/iconcodes.h"
#include "context/uicontext.h"
#include "context/shortcutcontext.h"

#include "dockwindow/idockwindow.h"
#include "async/notification.h"

#include "log.h"

using namespace kfw;
using namespace kn::appshell;
using namespace kfw::ui;
using namespace kfw::actions;
using namespace kfw::dock;

static const ActionCode FULL_SCREEN_CODE("fullscreen");
static const ActionCode TOGGLE_NAVIGATOR_ACTION_CODE("toggle-navigator");
static const ActionCode TOGGLE_BRAILLE_ACTION_CODE("toggle-braille-panel");
static const ActionCode TOGGLE_PERCUSSION_PANEL_ACTION_CODE("toggle-percussion-panel");

const UiActionList ApplicationUiActions::m_actions = {
    UiAction("quit",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Quit"),
             TranslatableString("action", "Quit")
             ),
    UiAction("restart",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Restart")
             ),
    UiAction(FULL_SCREEN_CODE,
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Full screen"),
             TranslatableString("action", "Full screen"),
             Checkable::Yes
             ),
    UiAction("about-musescore",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&About KitNote…")
             ),
    UiAction("about-qt",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "About &Qt…")
             ),
    UiAction("about-musicxml",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "About &MusicXML…")
             ),
    UiAction("kitnote-step-grid",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "Step &grid"),
             TranslatableString("action", "Show or hide the step grid: the selected measure as a grid of sixteenth notes")
             ),
    UiAction("kitnote-audio-to-score",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Drum score from a &recording…"),
             TranslatableString("action", "Separate the drums of a recording and write them as a score (needs the audio engine of Preferences > AI)")
             ),
    UiAction("kitnote-patterns",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "Pattern &library"),
             TranslatableString("action", "Show or hide the grooves, fills and rudiments, each with a picture of what it writes")
             ),
    UiAction("kitnote-song-structure",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "Song &structure"),
             TranslatableString("action", "Show or hide the sections of the chart: Intro, Verse, Chorus and so on")
             ),
    UiAction("kitnote-practice",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Practice with rising tempo"),
             TranslatableString("action", "Show or hide the practice card: loop the selection, faster with every pass")
             ),
    UiAction("kitnote-command-search",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Find a command…"),
             TranslatableString("action", "Search all commands by name and run one")
             ),
    UiAction("kitnote-guide",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "KitNote &user guide"),
             TranslatableString("action", "Open the KitNote user guide")
             ),
    UiAction("revert-factory",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Revert to &factory settings"),
             TranslatableString("action", "Revert to factory settings")
             ),

    // Docking
    UiAction("dock-restore-default-layout",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Restore the &default layout"),
             TranslatableString("action", "Restore the default layout")
             ),

    // Toolbars
    UiAction("toggle-transport",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Playback controls"),
             TranslatableString("action", "Show/hide playback controls"),
             Checkable::Yes
             ),
    UiAction("toggle-noteinput",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Note input"),
             TranslatableString("action", "Show/hide note input toolbar"),
             Checkable::Yes
             ),

    // Vertical panels
    UiAction("toggle-palettes",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Palettes"),
             TranslatableString("action", "Show/hide palettes"),
             Checkable::Yes
             ),
    UiAction("toggle-instruments",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Layout"),
             TranslatableString("action", "Show/hide layout panel"),
             Checkable::Yes
             ),
    UiAction("inspector",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "Propert&ies"),
             TranslatableString("action", "Show/hide properties"),
             Checkable::Yes
             ),
    UiAction("toggle-selection-filter",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "S&election filter"),
             TranslatableString("action", "Show/hide selection filter"),
             Checkable::Yes
             ),
    UiAction("toggle-undo-history-panel",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&History"),
             TranslatableString("action", "Show/hide undo history"),
             Checkable::Yes
             ),

    // Navigator
    UiAction(TOGGLE_NAVIGATOR_ACTION_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Navigator"),
             TranslatableString("action", "Show/hide navigator"),
             Checkable::Yes
             ),

    // Braille panel
    UiAction(TOGGLE_BRAILLE_ACTION_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Braille"),
             TranslatableString("action", "Show/hide braille panel"),
             Checkable::Yes
             ),

    // Horizontal panels
    UiAction("toggle-timeline",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "Tim&eline"),
             TranslatableString("action", "Show/hide timeline"),
             Checkable::Yes
             ),
    UiAction("toggle-mixer",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "Mixer"),
             TranslatableString("action", "Show/hide mixer"),
             IconCode::Code::MIXER,
             Checkable::Yes
             ),
    UiAction("toggle-piano-keyboard",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_ANY,
             TranslatableString("action", "Piano &keyboard"),
             TranslatableString("action", "Show/hide piano keyboard"),
             Checkable::Yes
             ),
    UiAction(TOGGLE_PERCUSSION_PANEL_ACTION_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Percussion"),
             TranslatableString("action", "Show/hide percussion panel"),
             Checkable::Yes
             ),
    UiAction("toggle-scorecmp-tool",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Score comparison tool"),
             Checkable::Yes
             ),

    // Status bar
    UiAction("toggle-statusbar",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Status bar"),
             TranslatableString("action", "Show/hide status bar"),
             Checkable::Yes
             ),

    UiAction("preference-dialog",
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Preferences…"),
             TranslatableString("action", "Preferences")
             ),

    UiAction("action://copy",
             { "action://notation/copy" },
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "&Copy"),
             TranslatableString("action", "Copy"),
             IconCode::Code::COPY
             ),
    UiAction("action://cut",
             { "action://notation/cut" },
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Cu&t"),
             TranslatableString("action", "Cut"),
             IconCode::Code::CUT
             ),
    UiAction("action://paste",
             { "action://notation/paste" },
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Past&e"),
             TranslatableString("action", "Paste"),
             IconCode::Code::PASTE
             ),
    UiAction("action://undo",
             { "action://notation/undo" },
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Undo"),
             TranslatableString("action", "Undo"),
             IconCode::Code::UNDO
             ),
    UiAction("action://redo",
             { "action://notation/redo" },
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "Redo"),
             TranslatableString("action", "Redo"),
             IconCode::Code::REDO
             ),
    UiAction("action://delete",
             { "action://notation/delete" },
             kn::context::UiCtxAny,
             kn::context::CTX_ANY,
             TranslatableString("action", "De&lete"),
             TranslatableString("action", "Delete"),
             IconCode::Code::DELETE_TANK
             ),
    UiAction("action://cancel",
             { "action://notation/cancel" },
             kn::context::UiCtxAny,
             kn::context::CTX_ANY
             ),
};

ApplicationUiActions::ApplicationUiActions(std::shared_ptr<ApplicationActionController> controller, const modularity::ContextPtr& iocCtx)
    : kfw::Contextable(iocCtx), m_controller(controller)
{
}

void ApplicationUiActions::init()
{
    mainWindow()->isFullScreenChanged().onNotify(this, [this]() {
        m_actionCheckedChanged.send({ FULL_SCREEN_CODE });
    });

    configuration()->isNotationNavigatorVisibleChanged().onNotify(this, [this]() {
        m_actionCheckedChanged.send({ TOGGLE_NAVIGATOR_ACTION_CODE });
    });

    brailleConfiguration()->braillePanelEnabledChanged().onNotify(this, [this]() {
        m_actionCheckedChanged.send({ TOGGLE_BRAILLE_ACTION_CODE });
    });

    dockWindowProvider()->windowChanged().onNotify(this, [this]() {
        listenOpenedDocksChanged(dockWindowProvider()->window());
    });

    notationSceneConfiguration()->useNewPercussionPanelChanged().onNotify(this, [this]() {
        m_actionEnabledChanged.send({ TOGGLE_PERCUSSION_PANEL_ACTION_CODE });
    });
}

void ApplicationUiActions::listenOpenedDocksChanged(IDockWindow* window)
{
    if (!window) {
        return;
    }

    window->docksOpenStatusChanged().onReceive(this, [this](const QStringList& dockNames) {
        ActionCodeList actions;

        for (const ActionCode& toggleDockAction : toggleDockActions().keys()) {
            const DockName& dockName = toggleDockActions()[toggleDockAction];

            if (dockNames.contains(dockName)) {
                actions.push_back(toggleDockAction);
            }
        }

        if (!actions.empty()) {
            m_actionCheckedChanged.send(actions);
        }
    });
}

const kfw::ui::UiActionList& ApplicationUiActions::actionsList() const
{
    return m_actions;
}

bool ApplicationUiActions::actionEnabled(const UiAction& act) const
{
    if (act.code == TOGGLE_PERCUSSION_PANEL_ACTION_CODE) {
        return notationSceneConfiguration()->useNewPercussionPanel();
    }

    return m_controller->canReceiveAction(act.code);
}

bool ApplicationUiActions::actionChecked(const UiAction& act) const
{
    if (act.code == FULL_SCREEN_CODE) {
        return mainWindow()->isFullScreen();
    }

    QMap<ActionCode, DockName> toggleDockActions = ApplicationUiActions::toggleDockActions();
    DockName dockName = toggleDockActions.value(act.code, DockName());

    if (dockName.isEmpty()) {
        return false;
    }

    if (dockName == NOTATION_NAVIGATOR_PANEL_NAME) {
        return configuration()->isNotationNavigatorVisible();
    }

    if (dockName == NOTATION_BRAILLE_PANEL_NAME) {
        return brailleConfiguration() && brailleConfiguration()->braillePanelEnabled();
    }

    const IDockWindow* window = dockWindowProvider() ? dockWindowProvider()->window() : nullptr;
    return window ? window->isDockOpen(dockName) : false;
}

kfw::async::Channel<ActionCodeList> ApplicationUiActions::actionEnabledChanged() const
{
    return m_actionEnabledChanged;
}

kfw::async::Channel<ActionCodeList> ApplicationUiActions::actionCheckedChanged() const
{
    return m_actionCheckedChanged;
}

const QMap<ActionCode, DockName>& ApplicationUiActions::toggleDockActions()
{
    static const QMap<ActionCode, DockName> actionsMap {
        { "toggle-transport", PLAYBACK_TOOLBAR_NAME },
        { "toggle-noteinput", NOTE_INPUT_BAR_NAME },

        { "toggle-palettes", PALETTES_PANEL_NAME },
        { "toggle-instruments", LAYOUT_PANEL_NAME },
        { "inspector", INSPECTOR_PANEL_NAME },
        { "toggle-selection-filter", SELECTION_FILTERS_PANEL_NAME },
        { "toggle-undo-history-panel", UNDO_HISTORY_PANEL_NAME },

        { TOGGLE_NAVIGATOR_ACTION_CODE, NOTATION_NAVIGATOR_PANEL_NAME },
        { TOGGLE_BRAILLE_ACTION_CODE, NOTATION_BRAILLE_PANEL_NAME },

        { "toggle-timeline", TIMELINE_PANEL_NAME },
        { "toggle-mixer", MIXER_PANEL_NAME },
        { "toggle-piano-keyboard", PIANO_KEYBOARD_PANEL_NAME },
        { TOGGLE_PERCUSSION_PANEL_ACTION_CODE, PERCUSSION_PANEL_NAME },

        { "toggle-statusbar", NOTATION_STATUSBAR_NAME },
    };

    return actionsMap;
}
