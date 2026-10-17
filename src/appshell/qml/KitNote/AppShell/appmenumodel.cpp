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
#include "appmenumodel.h"

#include "types/translatablestring.h"

#include "kfw_framework_config.h"

#ifdef KFW_MODULE_WORKSPACE
#include "workspace/qml/KitNote/Workspace/workspacesmenumodel.h"
#endif

#include "log.h"

using namespace kfw;
using namespace kn::appshell;
using namespace kfw::ui;
using namespace kfw::uicomponents;
using namespace kn::project;
using namespace kfw::workspace;
using namespace kfw::actions;

static const ActionCode TOGGLE_UNDO_HISTORY_PANEL_CODE = "toggle-undo-history-panel";
static const QString VIEW_TOGGLE_UNDO_HISTORY_PANEL_ITEM_ID = "view/toggle-undo-history-panel";

static QString makeId(const ActionCode& actionCode, int itemIndex)
{
    return QString::fromStdString(actionCode) + QString::number(itemIndex);
}

AppMenuModel::AppMenuModel(QObject* parent)
    : AbstractMenuModel(parent)
{
#ifdef KFW_MODULE_WORKSPACE
    m_workspacesMenuModel = std::make_shared<WorkspacesMenuModel>(this);
#endif
}

void AppMenuModel::load()
{
    TRACEFUNC;

    AbstractMenuModel::load();

#ifdef KFW_MODULE_WORKSPACE
    m_workspacesMenuModel->load();
#endif

    MenuItemList items {
        makeFileMenu(),
        makeEditMenu(),
        makeViewMenu(),
        makeAddMenu(),
        makeFormatMenu(),
        makeToolsMenu()
    };

    if (globalConfiguration()->devModeEnabled()) {
        items << makeHelpMenu(false);
        items << makeDiagnosticsMenu();
    } else {
        items << makeHelpMenu(true);
    }

    setItems(items);

    setupConnections();

    //! NOTE: removes some undesired platform-specific items
    //! (such as "Start Dictation" and "Special Characters" on macOS)
    appMenuModelHook()->onAppMenuInited();
}

bool AppMenuModel::isGlobalMenuAvailable()
{
    return uiConfiguration()->isGlobalMenuAvailable();
}

void AppMenuModel::setupConnections()
{
    recentFilesController()->recentFilesListChanged().onNotify(this, [this]() {
        MenuItem& recentScoreListItem = findMenu("menu-file-open");

        MenuItemList recentScoresList = makeRecentScoresItems();
        bool openRecentEnabled = !recentScoresList.empty();

        if (!recentScoresList.empty()) {
            recentScoresList = appendClearRecentSection(recentScoresList);
        }

        UiActionState state = recentScoreListItem.state();
        state.enabled = openRecentEnabled;
        recentScoreListItem.setState(state);

        recentScoreListItem.setSubitems(recentScoresList);
    });

#if 0 // KitNote: one layout, so no Workspaces menu (the status bar has no workspace button either)
    connect(m_workspacesMenuModel.get(), &WorkspacesMenuModel::itemsChanged, this, [this]() {
        MenuItem& workspacesItem = findMenu("menu-workspaces");
        workspacesItem.setSubitems(m_workspacesMenuModel->items());
    });
#endif

    globalContext()->currentNotationChanged().onNotify(this, [this]() {
        auto stack = undoStack();
        if (stack) {
            stack->stackChanged().onNotify(this, [this]() {
                updateUndoRedoItems();
            }, Asyncable::Mode::SetReplace /* FIXME */);
        }

        updateUndoRedoItems();
    });
}

void AppMenuModel::onActionsStateChanges(const kfw::actions::ActionCodeList& codes)
{
    AbstractMenuModel::onActionsStateChanges(codes);

    if (kfw::contains(codes, TOGGLE_UNDO_HISTORY_PANEL_CODE)) {
        // Appears both in Edit and View menus; AbstractMenuModel::onActionsStateChanges()
        // handles only one occurrence per action code
        MenuItem& item = findItem(VIEW_TOGGLE_UNDO_HISTORY_PANEL_ITEM_ID);
        if (item.isValid()) {
            item.setState(uiActionsRegister()->actionState(TOGGLE_UNDO_HISTORY_PANEL_CODE));
        }
    }
}

MenuItem* AppMenuModel::makeMenuItem(const ActionCode& actionCode, MenuItemRole menuRole)
{
    MenuItem* item = makeMenuItem(actionCode);
    item->setRole(menuRole);
    return item;
}

MenuItem* AppMenuModel::makeFileMenu()
{
    MenuItemList recentScoresList = makeRecentScoresItems();
    bool openRecentEnabled = !recentScoresList.isEmpty();

    if (!recentScoresList.empty()) {
        recentScoresList = appendClearRecentSection(recentScoresList);
    }

    MenuItemList fileItems {
        makeMenuItem("file-new"),
        makeMenuItem("file-open"),
        makeMenuItem("kitnote-audio-to-score"),     // KitNote
        makeMenu(TranslatableString("appshell/menu/file", "Open &recent"), recentScoresList, "menu-file-open", openRecentEnabled),
        makeMenuItem("file-close"),
        makeSeparator(),
        makeMenuItem("file-save"),
        makeMenuItem("file-save-as"),
        // KitNote: no cloud saving, online publishing or online import services,
        // and no parts dialog (a drum chart is a single part).
        makeMenu(TranslatableString("appshell/menu/file", "Save o&ther"), makeSaveOtherSubItems()),
        makeSeparator(),
        makeMenuItem("file-export"),
        makeSeparator(),
        makeMenuItem("project-properties"),
        makeMenuItem("kitnote-recording"),          // KitNote
        makeSeparator(),
        makeMenuItem("print"),
        makeSeparator(),
        makeMenuItem("quit", MenuItemRole::QuitRole)
    };

    return makeMenu(TranslatableString("appshell/menu/file", "&File"), fileItems, "menu-file");
}

MenuItemList AppMenuModel::makeSaveOtherSubItems()
{
    MenuItemList subItems {
        makeMenuItem("file-save-a-copy"),
        makeMenuItem("file-save-selection"),
    };
    return subItems;
}

MenuItem* AppMenuModel::makeEditMenu()
{
    MenuItemList editItems {
        makeMenuItem("action://notation/undo"),
        makeMenuItem("action://notation/redo"),
        makeMenuItem(TOGGLE_UNDO_HISTORY_PANEL_CODE),
        makeSeparator(),
        makeMenuItem("action://notation/cut"),
        makeMenuItem("action://notation/copy"),
        makeMenuItem("action://notation/paste"),
        makeMenuItem("notation-paste-half"),
        makeMenuItem("notation-paste-double"),
        makeMenuItem("notation-swap"),
        makeMenuItem("action://notation/delete"),
        makeSeparator(),
        makeMenuItem("notation-select-all"),
        makeMenuItem("notation-select-section"),
        makeMenuItem("find"),
        makeSeparator(),
        makeMenuItem("preference-dialog", MenuItemRole::PreferencesRole)
    };

    return makeMenu(TranslatableString("appshell/menu/edit", "&Edit"), editItems, "menu-edit");
}

kn::notation::INotationUndoStackPtr AppMenuModel::undoStack() const
{
    kn::notation::INotationPtr notation = globalContext()->currentNotation();
    return notation ? notation->undoStack() : nullptr;
}

void AppMenuModel::updateUndoRedoItems()
{
    auto stack = undoStack();

    MenuItem& undoItem = findItem(ActionCode("action://notation/undo"));
    const TranslatableString undoActionName = stack ? stack->topMostUndoActionName() : TranslatableString();
    undoItem.setTitle(undoActionName.isEmpty()
                      ? TranslatableString("action", "Undo")
                      : TranslatableString("action", "Undo ‘%1’").arg(undoActionName));

    MenuItem& redoItem = findItem(ActionCode("action://notation/redo"));
    const TranslatableString redoActionName = stack ? stack->topMostRedoActionName() : TranslatableString();
    redoItem.setTitle(redoActionName.isEmpty()
                      ? TranslatableString("action", "Redo")
                      : TranslatableString("action", "Redo ‘%1’").arg(redoActionName));
}

MenuItem* AppMenuModel::makeViewMenu()
{
    MenuItem* historyItem = makeMenuItem(TOGGLE_UNDO_HISTORY_PANEL_CODE);
    historyItem->setId(VIEW_TOGGLE_UNDO_HISTORY_PANEL_ITEM_ID);

    MenuItemList viewItems {
#ifndef Q_OS_MAC
        makeMenuItem("fullscreen"),
#endif
        makeMenuItem("toggle-palettes"),
        makeMenuItem("masterpalette"),
        makeMenuItem("toggle-instruments"),
        makeMenuItem("inspector"),
        makeMenuItem("toggle-selection-filter"),
        historyItem,
        makeMenuItem("toggle-navigator"),
        // KitNote: no Braille panel and no piano keyboard.
        makeMenuItem("toggle-timeline"),
        makeMenuItem("toggle-mixer"),
        makeMenuItem("toggle-percussion-panel"),
        makeMenuItem("playback-setup"),
        //makeMenuItem("toggle-scorecmp-tool"), // not implemented
        makeSeparator(),
        makeMenu(TranslatableString("appshell/menu/view", "&Toolbars"), makeToolbarsItems(), "menu-toolbars")
    };

#if 0 // KitNote: no Workspaces menu
    viewItems << makeMenu(TranslatableString("appshell/menu/view", "W&orkspaces"), m_workspacesMenuModel->items(), "menu-workspaces"),
#endif

    viewItems << makeSeparator()
              << makeMenu(TranslatableString("appshell/menu/view", "&Show"), makeShowItems(), "menu-show")
              << makeSeparator()
              << makeMenuItem("dock-restore-default-layout");

    return makeMenu(TranslatableString("appshell/menu/view", "&View"), viewItems, "menu-view");
}

MenuItem* AppMenuModel::makeAddMenu()
{
    MenuItemList addItems {
        // KitNote: no "Intervals" and no "Chords and fretboard diagrams"; they act on pitch.
        makeMenu(TranslatableString("appshell/menu/add", "&Notes"), makeNotesItems(), "menu-notes"),
        makeMenu(TranslatableString("appshell/menu/add", "T&uplets"), makeTupletsItems(), "menu-tuplets"),
        makeSeparator(),
        makeMenu(TranslatableString("appshell/menu/add", "&Measures"), makeMeasuresItems(), "menu-measures"),
        makeMenu(TranslatableString("appshell/menu/add", "&Frames"), makeFramesItems(), "menu-frames"),
        makeMenu(TranslatableString("appshell/menu/add", "&Text"), makeTextItems(), "menu-text"),
        makeMenu(TranslatableString("appshell/menu/add", "&Lines"), makeLinesItems(), "menu-lines"),
        makeSeparator(),
        // KitNote: grooves and fills written into the selected measures.
        makeMenu(TranslatableString("appshell/menu/add", "&Drum patterns"), makeDrumPatternItems(), "menu-drum-patterns"),
        makeMenu(TranslatableString("appshell/menu/add", "R&udiments"), makeDrumRudimentItems(), "menu-drum-rudiments"),
        makeMenuItem("drum-key-insert"),
        makeMenuItem("drum-key-remove"),
    };

    return makeMenu(TranslatableString("appshell/menu/add", "&Add"), addItems, "menu-add");
}

MenuItem* AppMenuModel::makeFormatMenu()
{
    MenuItemList stretchItems {
        makeMenuItem("stretch+"),
        makeMenuItem("stretch-"),
        makeMenuItem("reset-stretch")
    };

    MenuItemList formatItems {
        makeMenuItem("edit-style"),
        makeMenuItem("page-settings"),
        makeSeparator(),
        makeMenuItem("measures-per-system"),
        makeMenu(TranslatableString("appshell/menu/format", "Str&etch"), stretchItems, "menu-stretch"),
        // KitNote: the size of the staves, without the page settings dialog
        makeMenu(TranslatableString("appshell/menu/format", "Staff si&ze"), {
            makeMenuItem("staff-size-small"),
            makeMenuItem("staff-size-normal"),
            makeMenuItem("staff-size-large"),
        }, "menu-staff-size"),
        makeSeparator(),
        makeMenuItem("reset-text-style-overrides"),
        makeMenuItem("reset-beammode"),
        makeMenuItem("reset"),
        makeMenuItem("reset-to-default-layout"),
        makeSeparator(),
        makeMenuItem("load-style"),
        makeMenuItem("save-style")
    };

    return makeMenu(TranslatableString("appshell/menu/format", "F&ormat"), formatItems, "menu-format");
}

MenuItem* AppMenuModel::makeToolsMenu()
{
    MenuItemList voicesItems {
        makeMenuItem("voice-x12"),
        makeMenuItem("voice-x13"),
        makeMenuItem("voice-x14"),
        makeMenuItem("voice-x23"),
        makeMenuItem("voice-x24"),
        makeMenuItem("voice-x34")
    };

    MenuItemList measuresItems {
        makeMenuItem("split-measure"),
        makeMenuItem("join-measures")
    };

    // KitNote: transposing, explode/implode, chord-symbol realisation and
    // pitch respelling have nothing to act on in a drum chart.
    MenuItemList toolsItems {
        makeMenu(TranslatableString("appshell/menu/tools", "&Voices"), voicesItems, "menu-voices"),
        makeMenu(TranslatableString("appshell/menu/tools", "&Measures"), measuresItems, "menu-tools-measures"),
        makeMenuItem("time-delete"),
        makeSeparator(),
        makeMenuItem("slash-fill"),
        makeMenuItem("slash-rhythm"),
        makeSeparator(),
        // KitNote: how playback swings.
        makeMenu(TranslatableString("appshell/menu/tools", "S&wing"), {
            makeMenuItem("swing-off"),
            makeMenuItem("swing-eighth"),
            makeMenuItem("swing-sixteenth"),
        }, "menu-swing"),
        // KitNote: R L R L under the selected notes.
        makeMenuItem("sticking-alternate-right"),
        makeMenuItem("sticking-alternate-left"),
        makeMenuItem("counting-add"),
        // KitNote: changes to the selected drum notes
        makeMenu(TranslatableString("appshell/menu/tools", "C&hange the selection"), {
            makeMenuItem("drum-change-hihat-to-ride"),
            makeMenuItem("drum-change-ride-to-hihat"),
            makeSeparator(),
            makeMenuItem("drum-change-swap-sticking"),
            makeSeparator(),
            makeMenuItem("drum-change-accent-beats"),
            makeMenuItem("drum-change-accent-offbeats"),
            makeMenuItem("drum-change-accent-clear"),
        }, "menu-drum-change"),
        makeMenuItem("measure-repeat-identical"),
        makeMenuItem("measure-repeat-write-out"),
        makeSeparator(),
        makeMenuItem("reset-groupings"),
        makeMenuItem("resequence-rehearsal-marks"),
        /*
         * TODO: https://github.com/musescore/MuseScore/issues/9670
        makeMenuItem("unroll-repeats"),
         */
        makeSeparator(),
        makeMenuItem("drum-tab-copy"),
        makeMenuItem("drum-tab-paste"),
        makeMenuItem("copy-lyrics-to-clipboard"),
        makeMenuItem("del-empty-measures"),
    };

    return makeMenu(TranslatableString("appshell/menu/tools", "&Tools"), toolsItems, "menu-tools");
}

MenuItem* AppMenuModel::makeHelpMenu(bool addDiagnosticsSubMenu)
{
    MenuItemList helpItems;

    if (updateConfiguration()->isAppUpdatable()) {
        helpItems << makeMenuItem("check-update");
        helpItems << makeSeparator();
    }

    helpItems << makeMenuItem("kitnote-command-search");
    helpItems << makeMenuItem("kitnote-step-grid");
    helpItems << makeMenuItem("kitnote-patterns");
    helpItems << makeMenuItem("kitnote-practice");
    helpItems << makeMenuItem("kitnote-song-structure");
    helpItems << makeSeparator();
    helpItems << makeMenuItem("kitnote-guide");
    helpItems << makeSeparator();

    if (addDiagnosticsSubMenu) {
        helpItems << makeDiagnosticsMenu();
        helpItems << makeSeparator();
    }

    helpItems << makeMenuItem("about-musescore", MenuItemRole::AboutRole);
    helpItems << makeMenuItem("about-qt", MenuItemRole::AboutQtRole);
    helpItems << makeMenuItem("about-musicxml");
    helpItems << makeSeparator();

    helpItems << makeMenuItem("revert-factory");

    return makeMenu(TranslatableString("appshell/menu/help", "&Help"), helpItems, "menu-help");
}

MenuItem* AppMenuModel::makeDiagnosticsMenu()
{
    MenuItemList systemItems {
        makeMenuItem("diagnostic-show-paths"),
        makeMenuItem("diagnostic-show-graphicsinfo"),
        makeMenuItem("diagnostic-show-profiler"),
    };

    MenuItemList items {
        makeMenuItem("diagnostic-save-diagnostic-files"),
        makeMenuItem("playback-reload-cache"),
        makeMenu(TranslatableString("appshell/menu/diagnostics", "&System"), systemItems, "menu-system")
    };

    if (globalConfiguration()->devModeEnabled()) {
        MenuItemList actionsItems {
            makeMenuItem("diagnostic-show-actions"),
            makeMenuItem("action://diagnostic/actions/query"),
            makeMenuItem("action://diagnostic/actions/query_params1?param1=val1"),
            makeMenuItem("action://diagnostic/actions/query_params2?param1=val1")
        };

        MenuItemList accessibilityItems {
            makeMenuItem("diagnostic-show-navigation-tree"),
            makeMenuItem("diagnostic-show-accessible-tree"),
            makeMenuItem("diagnostic-accessible-tree-dump"),
        };

        MenuItemList engravingItems {
            makeMenuItem("diagnostic-show-engraving-elements"),
            makeMenuItem("diagnostic-show-engraving-undostack"),
            makeMenuItem("diagnostic-show-engraving-style"),
            makeSeparator(),
            makeMenuItem("show-element-bounding-rects"),
            makeMenuItem("color-element-shapes"),
            makeMenuItem("show-segment-shapes"),
            makeMenuItem("color-segment-shapes"),
            makeMenuItem("show-skylines"),
            makeMenuItem("show-system-bounding-rects"),
            makeMenuItem("show-element-masks"),
            makeMenuItem("show-gap-rests"),
            makeMenuItem("show-line-attach-points"),
            makeMenuItem("mark-empty-staff-visibility-overrides"),
            makeMenuItem("mark-corrupted-measures"),
            makeMenuItem("check-for-score-corruptions")
        };

#ifdef KFW_MODULE_VST
        MenuItemList vstItems {
            makeMenuItem("vst-use-oldview"),
            makeMenuItem("vst-use-newview"),
        };
#endif

        MenuItemList audioItems {
            makeMenuItem("action://audio/dev/use-workermode"),
            makeMenuItem("action://audio/dev/use-drivermode"),
            makeMenuItem("action://audio/dev/use-workerrpcmode"),
        };

        items << makeMenu(TranslatableString("appshell/menu/diagnostics", "A&ctions"), actionsItems, "menu-actions")
              << makeMenu(TranslatableString("appshell/menu/diagnostics", "&Accessibility"), accessibilityItems, "menu-accessibility")
              << makeMenu(TranslatableString("appshell/menu/diagnostics", "&Engraving"), engravingItems, "menu-engraving");

#ifdef KFW_MODULE_VST
        items << makeMenu(TranslatableString("appshell/menu/diagnostics", "&VST"), vstItems, "menu-vst");
#endif

        items << makeMenu(TranslatableString("appshell/menu/diagnostics", "&Audio"), audioItems, "menu-audio")
              << makeMenuItem("multiwindows-dev-show-info");
    }

    return makeMenu(TranslatableString("appshell/menu/diagnostics", "&Diagnostics"), items, "menu-diagnostic");
}

MenuItemList AppMenuModel::makeRecentScoresItems()
{
    MenuItemList items;
    const RecentFilesList& recentFiles = recentFilesController()->recentFilesList();

    int index = 0;
    for (const RecentFile& file : recentFiles) {
        MenuItem* item = new MenuItem(this);

        UiAction action;
        action.code = "file-open";
        action.title = TranslatableString::untranslatable(file.displayName(/*includingExtension*/ true));
        item->setAction(action);

        item->setId(makeId(item->action().code, index++));

        UiActionState state;
        state.enabled = true;
        item->setState(state);

        item->setSelectable(true);
        item->setArgs(ActionData::make_arg2<QUrl, QString>(file.path.toQUrl(), file.displayNameOverride));

        items << item;
    }

    return items;
}

MenuItemList AppMenuModel::appendClearRecentSection(const kfw::uicomponents::MenuItemList& recentScores)
{
    MenuItemList result = recentScores;
    result << makeSeparator()
           << makeMenuItem("clear-recent");

    return result;
}

MenuItemList AppMenuModel::makeNotesItems()
{
    MenuItemList items {
        makeMenuItem("note-input"),
        makeSeparator(),
        makeMenuItem("note-c"),
        makeMenuItem("note-d"),
        makeMenuItem("note-e"),
        makeMenuItem("note-f"),
        makeMenuItem("note-g"),
        makeMenuItem("note-a"),
        makeMenuItem("note-b"),
        makeSeparator(),
        makeMenuItem("chord-c"),
        makeMenuItem("chord-d"),
        makeMenuItem("chord-e"),
        makeMenuItem("chord-f"),
        makeMenuItem("chord-g"),
        makeMenuItem("chord-a"),
        makeMenuItem("chord-b")
    };

    return items;
}

MenuItemList AppMenuModel::makeTupletsItems()
{
    MenuItemList items {
        makeMenuItem("duplet"),
        makeMenuItem("triplet"),
        makeMenuItem("quadruplet"),
        makeMenuItem("quintuplet"),
        makeMenuItem("sextuplet"),
        makeMenuItem("septuplet"),
        makeMenuItem("octuplet"),
        makeMenuItem("nonuplet"),
        makeMenuItem("tuplet-dialog")
    };

    return items;
}

MenuItemList AppMenuModel::makeMeasuresItems()
{
    MenuItemList items {
        makeMenuItem("insert-measure", TranslatableString("notation", "&Insert one measure before selection")),
        makeMenuItem("append-measure", TranslatableString("notation", "Insert &one measure at end of score")),
        makeSeparator(),
        makeMenuItem("insert-measures", TranslatableString("notation", "Insert &before selection…")),
        makeMenuItem("insert-measures-after-selection", TranslatableString("notation", "Insert &after selection…")),
        makeSeparator(),
        makeMenuItem("insert-measures-at-start-of-score", TranslatableString("notation", "Insert at &start of score…")),
        makeMenuItem("append-measures", TranslatableString("notation", "Insert at &end of score…"))
    };

    return items;
}

// KitNote: snare rudiments, written with their sticking.
MenuItemList AppMenuModel::makeDrumRudimentItems()
{
    MenuItemList items {
        makeMenuItem("drum-rudiment-single-stroke"),
        makeMenuItem("drum-rudiment-double-stroke"),
        makeMenuItem("drum-rudiment-paradiddle"),
        makeMenuItem("drum-rudiment-stroke-triplets"),
        makeMenuItem("drum-rudiment-double-paradiddle"),
        makeMenuItem("drum-rudiment-paradiddle-diddle"),
        makeSeparator(),
        makeMenuItem("drum-rudiment-flams"),
        makeMenuItem("drum-rudiment-flam-tap"),
        makeMenuItem("drum-rudiment-flam-accent")
    };

    return items;
}

MenuItemList AppMenuModel::makeDrumPatternItems()
{
    MenuItemList items {
        makeMenuItem("drum-pattern-rock-8"),
        makeMenuItem("drum-pattern-rock-8-push"),
        makeMenuItem("drum-pattern-rock-16"),
        makeMenuItem("drum-pattern-quarter"),
        makeMenuItem("drum-pattern-four-floor"),
        makeMenuItem("drum-pattern-half-time"),
        makeMenuItem("drum-pattern-ride-8"),
        makeMenuItem("drum-pattern-crash-rock-8"),
        makeMenuItem("drum-pattern-motown"),
        makeSeparator(),
        makeMenuItem("drum-pattern-shuffle"),
        makeMenuItem("drum-pattern-triplet-feel"),
        makeMenuItem("drum-pattern-jazz-ride"),
        makeSeparator(),
        makeMenuItem("drum-pattern-disco"),
        makeMenuItem("drum-pattern-bossa-nova"),
        makeMenuItem("drum-pattern-reggae"),
        makeMenuItem("drum-pattern-six-eight"),
        makeSeparator(),
        makeMenuItem("drum-pattern-snare-fill"),
        makeMenuItem("drum-pattern-tom-fill"),
        makeMenuItem("drum-pattern-triplet-fill"),
        makeMenuItem("drum-pattern-half-bar-fill")
    };

    return items;
}

MenuItemList AppMenuModel::makeFramesItems()
{
    MenuItemList items {
        makeMenuItem("insert-hbox"),
        makeMenuItem("insert-vbox"),
        makeMenuItem("insert-textframe"),
        makeMenuItem("insert-fretframe"),
        makeSeparator(),
        makeMenu(TranslatableString("notation", "Insert at end of score"), makeFramesAppendItems())
    };

    return items;
}

MenuItemList AppMenuModel::makeFramesAppendItems()
{
    MenuItemList items {
        makeMenuItem("append-hbox"),
        makeMenuItem("append-vbox"),
        makeMenuItem("append-textframe"),
        makeMenuItem("append-fretframe")
    };

    return items;
}

MenuItemList AppMenuModel::makeTextItems()
{
    MenuItemList items {
        makeMenuItem("title-text"),
        makeMenuItem("subtitle-text"),
        makeMenuItem("composer-text"),
        makeMenuItem("poet-text"),
        makeMenuItem("part-text"),
        makeSeparator(),
        makeMenuItem("system-text"),
        makeMenuItem("staff-text"),
        makeMenuItem("add-dynamic"),
        makeMenuItem("expression-text"),
        makeMenuItem("rehearsalmark-text"),
        makeMenuItem("instrument-change-text"),
        makeMenuItem("fingering-text"),
        makeSeparator(),
        makeMenuItem("sticking-text"),
        makeMenuItem("chord-text"),
        makeMenuItem("roman-numeral-text"),
        makeMenuItem("nashville-number-text"),
        makeMenuItem("lyrics"),
        makeMenuItem("figured-bass"),
        makeMenuItem("tempo")
    };

    return items;
}

MenuItemList AppMenuModel::makeLinesItems()
{
    MenuItemList items {
        makeMenuItem("add-slur"),
        makeMenuItem("add-hairpin"),
        makeMenuItem("add-hairpin-reverse"),
        makeMenuItem("add-8va"),
        makeMenuItem("add-8vb"),
        makeMenuItem("add-noteline")
    };

    return items;
}

MenuItemList AppMenuModel::makeToolbarsItems()
{
    MenuItemList items {
        makeMenuItem("toggle-transport"),
        makeMenuItem("toggle-noteinput"),
        makeMenuItem("toggle-statusbar")
    };

    return items;
}

MenuItemList AppMenuModel::makeShowItems()
{
    MenuItemList items {
        makeMenuItem("show-invisible"),
        makeMenuItem("show-unprintable"),
        makeMenuItem("show-frames"),
        makeMenuItem("show-pageborders"),
        makeMenuItem("show-irregular"),
        makeMenuItem("show-soundflags"),
    };

    return items;
}
