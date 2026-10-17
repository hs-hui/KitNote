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
#include "notationuiactions.h"

#include "engraving/types/typesconv.h" // KitNote

#include <unordered_map>

#include "types/translatablestring.h"
#include "ui/view/iconcodes.h"
#include "context/shortcutcontext.h"

using namespace kn;
using namespace kn::notation;
using namespace kfw;
using namespace kfw::ui;
using namespace kfw::actions;

static const ActionCode NOTE_INPUT_ACTION_CODE("note-input");

static const ActionCode SHOW_INVISIBLE_CODE("show-invisible");
static const ActionCode SHOW_UNPRINTABLE_CODE("show-unprintable");
static const ActionCode SHOW_FRAMES_CODE("show-frames");
static const ActionCode SHOW_PAGEBORDERS_CODE("show-pageborders");
static const ActionCode SHOW_SOUND_FLAGS("show-soundflags");
static const ActionCode SHOW_IRREGULAR_CODE("show-irregular");

static const ActionCode TOGGLE_CONCERT_PITCH_CODE("concert-pitch");
// KitNote
static const ActionCode SWING_OFF_CODE("swing-off");
static const ActionCode SWING_EIGHTH_CODE("swing-eighth");
static const ActionCode SWING_SIXTEENTH_CODE("swing-sixteenth");

// avoid translation duplication

// //: This is comment for translator

//: Note
static const TranslatableString noteC = TranslatableString("action", "C");
//: Note
static const TranslatableString noteD = TranslatableString("action", "D");
//: Note
static const TranslatableString noteE = TranslatableString("action", "E");
//: Note
static const TranslatableString noteF = TranslatableString("action", "F");
//: Note
static const TranslatableString noteG = TranslatableString("action", "G");
//: Note
static const TranslatableString noteA = TranslatableString("action", "A");
//: Note
static const TranslatableString noteB = TranslatableString("action", "B");

static const TranslatableString Enter_note_X = TranslatableString("action", "Enter note %1");
static const TranslatableString Add_X_to_chord = TranslatableString("action", "Add %1 to chord");
static const TranslatableString Add_note_X_to_chord = TranslatableString("action", "Add note %1 to chord");
static const TranslatableString Insert_X = TranslatableString("action", "Insert %1");

static const TranslatableString fret_X_TAB = TranslatableString("action", "Fret %1 (TAB)");
static const TranslatableString enter_TAB_fret_X = TranslatableString("action", "Enter TAB: fret %1");

//: Addition to the name of an action to indicate that this action only applies to tablature notation.
//: '%1' is the name of the action.
static const TranslatableString X_TAB = TranslatableString("action", "%1 (TAB)");

//! NOTE Each notation actions should has context is UiCtxProjectOpened.
//! If you want what action to dispatch by shortcut only when notation is focused (ex notation-move-right by press Right key),
//! then you should set the shortcut context accordingly, not the action context.
//! Because actions can be dispatched not only shortcuts, but another way, ex by click Button, Menu and etc

const UiActionList NotationUiActions::s_actions = {
    UiAction("action://notation/copy",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_DISABLED,
             TranslatableString("action", "&Copy"),
             TranslatableString("action", "Copy"),
             IconCode::Code::COPY
             ),
    UiAction("action://notation/cut",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_DISABLED,
             TranslatableString("action", "Cu&t"),
             TranslatableString("action", "Cut"),
             IconCode::Code::CUT
             ),
    UiAction("action://notation/paste",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_DISABLED,
             TranslatableString("action", "Past&e"),
             TranslatableString("action", "Paste"),
             IconCode::Code::PASTE
             ),
    UiAction("action://notation/delete",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_DISABLED,
             TranslatableString("action", "De&lete"),
             TranslatableString("action", "Delete"),
             IconCode::Code::DELETE_TANK
             ),
    UiAction("action://notation/cancel",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_DISABLED
             ),

    UiAction("put-note", // args: PointF pos, bool replace, bool insert
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Put note")
             ),
    UiAction("remove-note", // args: PointF pos
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Remove note")
             ),
    UiAction("next-element",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Next element"),
             TranslatableString("action", "Select next element in score")
             ),
    UiAction("prev-element",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Previous element"),
             TranslatableString("action", "Select previous element in score")
             ),
    UiAction("notation-move-right",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Next chord / Shift text right"),
             TranslatableString("action", "Select next chord / move text right")
             ),
    UiAction("notation-move-left",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Previous chord / Shift text left"),
             TranslatableString("action", "Select previous chord / move text left")
             ),
    UiAction("notation-move-right-quickly",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Next measure / Shift text right quickly"),
             TranslatableString("action", "Go to next measure / move text right quickly")
             ),
    UiAction("notation-move-left-quickly",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Previous measure / Shift text left quickly"),
             TranslatableString("action", "Go to previous measure / move text left quickly")
             ),
    UiAction("up-chord",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_LIST_SELECTION,
             TranslatableString("action", "Up note in chord"),
             TranslatableString("action", "Select note/rest above")
             ),
    UiAction("down-chord",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_LIST_SELECTION,
             TranslatableString("action", "Down note in chord"),
             TranslatableString("action", "Select note/rest below")
             ),
    UiAction("top-chord",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Top note in chord"),
             TranslatableString("action", "Select top note in chord")
             ),
    UiAction("bottom-chord",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Bottom note in chord"),
             TranslatableString("action", "Select bottom note in chord")
             ),
    UiAction("first-element",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "First element"),
             TranslatableString("action", "Go to first element in score")
             ),
    UiAction("last-element",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Last element"),
             TranslatableString("action", "Go to last element in score")
             ),
    UiAction("move-up",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Move to staff above"),
             TranslatableString("action", "Move selected note/rest to staff above")
             ),
    UiAction("move-down",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Move to staff below"),
             TranslatableString("action", "Move selected note/rest to staff below")
             ),
    UiAction("next-track",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Next staff or voice"),
             TranslatableString("action", "Go to next staff or voice")
             ),
    UiAction("prev-track",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Previous staff or voice"),
             TranslatableString("action", "Go to previous staff or voice")
             ),
    UiAction("next-frame",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Next frame"),
             TranslatableString("action", "Go to next frame")
             ),
    UiAction("prev-frame",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Previous frame"),
             TranslatableString("action", "Go to previous frame")
             ),
    UiAction("next-system",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Next system"),
             TranslatableString("action", "Go to next system")
             ),
    UiAction("prev-system",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Previous system"),
             TranslatableString("action", "Go to previous system")
             ),
    UiAction("select-next-chord",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add next chord to selection"),
             TranslatableString("action", "Add to selection: next note/rest")
             ),
    UiAction("select-prev-chord",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add previous chord to selection"),
             TranslatableString("action", "Add to selection: previous note/rest")
             ),
    UiAction("move-left",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Move chord/rest left"),
             TranslatableString("action", "Move chord/rest left")
             ),
    UiAction("move-right",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Move chord/rest right"),
             TranslatableString("action", "Move chord/rest right")
             ),
    UiAction("toggle-snap-to-previous",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Snap to &previous"),
             TranslatableString("action", "Snap to previous"),
             Checkable::Yes
             ),
    UiAction("toggle-snap-to-next",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Snap to &next"),
             TranslatableString("action", "Snap to next"),
             Checkable::Yes
             ),
    UiAction("pitch-up",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "Up"),
             TranslatableString("action", "Move pitch/selection up")
             ),
    UiAction("pitch-down",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "Down"),
             TranslatableString("action", "Move pitch/selection down")
             ),
    UiAction("pitch-down-octave",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Down octave"),
             TranslatableString("action", "Move pitch down an octave")
             ),
    UiAction("pitch-up-octave",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Up octave"),
             TranslatableString("action", "Move pitch up an octave")
             ),
    UiAction("double-duration",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Double duration"),
             TranslatableString("action", "Double selected duration")
             ),
    UiAction("half-duration",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Halve duration"),
             TranslatableString("action", "Halve selected duration")
             ),
    UiAction("inc-duration-dotted",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Double selected duration (dotted)"),
             TranslatableString("action", "Double selected duration (includes dotted values)")
             ),
    UiAction("dec-duration-dotted",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Halve selected duration (dotted)"),
             TranslatableString("action", "Halve selected duration (includes dotted values)")
             ),
    UiAction("notation-paste-half",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Paste &half duration"),
             TranslatableString("action", "Paste half duration")
             ),
    UiAction("notation-paste-double",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Paste &double duration"),
             TranslatableString("action", "Paste double duration")
             ),
    UiAction("notation-paste-special",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Paste special"),
             TranslatableString("action", "Paste special")
             ),
    UiAction("notation-swap",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "&Swap with clipboard"),
             TranslatableString("action", "Copy/paste: swap with clipboard")
             ),
    UiAction("toggle-visible",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle visibility of elements"),
             TranslatableString("action", "Toggle visibility of elements")
             ),
    UiAction("notation-select-all",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Select &all"),
             TranslatableString("action", "Select all")
             ),
    UiAction("notation-select-section",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Select sectio&n"),
             TranslatableString("action", "Select section")
             ),
    UiAction("select-similar",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Similar"),
             TranslatableString("action", "Select similar elements")
             ),
    UiAction("select-similar-staff",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Similar on this staff"),
             TranslatableString("action", "Select similar elements on the same staff")
             ),
    UiAction("select-similar-range",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Similar in this range"),
             TranslatableString("action", "Select similar elements in the selected range")
             ),
    UiAction("select-dialog",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "More…"),
             TranslatableString("action", "Select similar elements with more options")
             ),
    UiAction("edit-style",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Style…"),
             TranslatableString("action", "Format style")
             ),
    UiAction("page-settings",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Page settings…"),
             TranslatableString("action", "Page settings")
             ),
    UiAction("load-style",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Load style…"),
             TranslatableString("action", "Load style")
             ),
    UiAction("save-style",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "S&ave style…"),
             TranslatableString("action", "Save style")
             ),
    UiAction("transpose",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Transpose…"),
             TranslatableString("action", "Transpose")
             ),
    UiAction("explode",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Explode"),
             TranslatableString("action", "Explode")
             ),
    UiAction("implode",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Implode"),
             TranslatableString("action", "Implode")
             ),
    UiAction("add-fretboard-diagram",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Add &fretboard diagram"),
             TranslatableString("action", "Add fretboard diagram"),
             IconCode::Code::FRETBOARD_DIAGRAM
             ),
    UiAction("realize-chord-symbols",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Realize &chord symbols"),
             TranslatableString("action", "Realize chord symbols")
             ),
    UiAction("extend-to-next-note",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Extend to next note"),
             TranslatableString("action", "Extend to next note")
             ),
    UiAction("time-delete",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Remove selected ran&ge"),
             TranslatableString("action", "Delete selected measures"),
             IconCode::Code::DELETE_TANK
             ),
    // KitNote: repeat signs for repeated measures
    UiAction("measure-repeat-identical",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Replace repeated measures with repeat signs"),
             TranslatableString("action", "Replace each measure, or phrase of two or four measures, that repeats what comes "
                                          "just before it with a measure-repeat sign")
             ),
    UiAction("measure-repeat-write-out",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Write out measure-repeat signs"),
             TranslatableString("action", "Replace each measure-repeat sign with the notes it stands for")
             ),
    UiAction("drum-key-remove",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Remove the drum key"),
             TranslatableString("action", "Remove the key of the drums from the start of the score")
             ),
    UiAction("drum-key-insert",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Drum &key at the start of the score"),
             TranslatableString("action", "Insert a key of the drums used, with their names, before the first measure")
             ),
    // KitNote: swing
    UiAction(SWING_OFF_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Straight (no swing)"),
             TranslatableString("action", "Play the score as written, without swing"),
             Checkable::Yes
             ),
    UiAction(SWING_EIGHTH_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Swung &eighth notes"),
             TranslatableString("action", "Play pairs of eighth notes with swing"),
             Checkable::Yes
             ),
    UiAction(SWING_SIXTEENTH_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Swung si&xteenth notes"),
             TranslatableString("action", "Play pairs of sixteenth notes with swing"),
             Checkable::Yes
             ),
    // KitNote: the size of the staves
    UiAction("staff-size-small",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Small staves"),
             TranslatableString("action", "Staff size: small (1.5 mm between the lines) - more measures to a page")
             ),
    UiAction("staff-size-normal",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Normal staves"),
             TranslatableString("action", "Staff size: normal (1.75 mm between the lines)")
             ),
    UiAction("staff-size-large",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Large staves"),
             TranslatableString("action", "Staff size: large (2.1 mm between the lines) - for reading from a music stand")
             ),
    // KitNote: alternating sticking
    UiAction("sticking-alternate-right",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Sticking: R L R L"),
             TranslatableString("action", "Add alternating sticking to the selected notes, starting with the right hand")
             ),
    UiAction("sticking-alternate-left",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Sticking: L R L R"),
             TranslatableString("action", "Add alternating sticking to the selected notes, starting with the left hand")
             ),
    // KitNote: changes to the selected drum notes
    UiAction("drum-change-hihat-to-ride",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Hi-hat to &ride"),
             TranslatableString("action", "Move the hi-hat notes of the selection to the ride cymbal")
             ),
    UiAction("drum-change-ride-to-hihat",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Ride to &hi-hat"),
             TranslatableString("action", "Move the ride notes of the selection to the hi-hat")
             ),
    UiAction("drum-change-swap-sticking",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Swap the sticking (R and L)"),
             TranslatableString("action", "Exchange R and L in the sticking of the selection, for the other hand to lead")
             ),
    UiAction("drum-change-accent-beats",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Accents &on the beats"),
             TranslatableString("action", "Accent the strokes of the selection that fall on a beat")
             ),
    UiAction("drum-change-accent-offbeats",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Accents o&ff the beats"),
             TranslatableString("action", "Accent the strokes of the selection that fall halfway between two beats")
             ),
    UiAction("drum-change-accent-clear",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Remove the accents"),
             TranslatableString("action", "Remove the accents from the selection")
             ),
    // KitNote: counting
    UiAction("counting-add",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Counting: 1 e && a"),
             TranslatableString("action", "Write the count (1 e & a) under the selected notes")
             ),
    // KitNote: rudiments
    UiAction("drum-rudiment-single-stroke",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Single stroke roll (R L R L)"),
             TranslatableString("action", "Fill the selected measures: single stroke roll")
             ),
    UiAction("drum-rudiment-double-stroke",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Double stroke roll (R R L L)"),
             TranslatableString("action", "Fill the selected measures: double stroke roll")
             ),
    UiAction("drum-rudiment-paradiddle",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Single paradiddle (R L R R L R L L)"),
             TranslatableString("action", "Fill the selected measures: single paradiddle")
             ),
    UiAction("drum-rudiment-stroke-triplets",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Single stroke triplets (R L R L R L)"),
             TranslatableString("action", "Fill the selected measures: single stroke triplets")
             ),
    UiAction("drum-rudiment-double-paradiddle",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Double paradiddle (R L R L R R L R L R L L)"),
             TranslatableString("action", "Fill the selected measures: double paradiddle")
             ),
    UiAction("drum-rudiment-paradiddle-diddle",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Paradiddle-diddle (R L R R L L)"),
             TranslatableString("action", "Fill the selected measures: paradiddle-diddle")
             ),
    UiAction("drum-rudiment-flams",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Flams (lR rL)"),
             TranslatableString("action", "Fill the selected measures: flams")
             ),
    UiAction("drum-rudiment-flam-tap",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Flam tap (lR R rL L)"),
             TranslatableString("action", "Fill the selected measures: flam tap")
             ),
    UiAction("drum-rudiment-flam-accent",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Flam accent (lR L R rL R L)"),
             TranslatableString("action", "Fill the selected measures: flam accent")
             ),
    // KitNote: drum patterns
    UiAction("drum-pattern-disco",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Disco beat (open hi-hat off the beat)"),
             TranslatableString("action", "Fill the selected measures: disco beat (open hi-hat off the beat)")
             ),
    UiAction("drum-pattern-motown",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Motown beat (snare on every beat)"),
             TranslatableString("action", "Fill the selected measures: Motown beat")
             ),
    UiAction("drum-pattern-jazz-ride",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Jazz ride (swing)"),
             TranslatableString("action", "Fill the selected measures: jazz ride")
             ),
    UiAction("drum-pattern-reggae",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Reggae one drop"),
             TranslatableString("action", "Fill the selected measures: reggae one drop")
             ),
    UiAction("drum-pattern-half-bar-fill",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Fill: beat, then a fill on 3 and 4"),
             TranslatableString("action", "Fill the selected measures: half a measure of beat and half a measure of fill")
             ),
    UiAction("drum-pattern-bossa-nova",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Bossa nova"),
             TranslatableString("action", "Fill the selected measures: bossa nova")
             ),
    UiAction("drum-pattern-six-eight",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "6/8 beat"),
             TranslatableString("action", "Fill the selected measures: 6/8 beat")
             ),
    UiAction("drum-pattern-shuffle",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Shuffle beat"),
             TranslatableString("action", "Fill the selected measures: shuffle beat")
             ),
    UiAction("drum-pattern-triplet-feel",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Slow blues beat (triplet hi-hat)"),
             TranslatableString("action", "Fill the selected measures: slow blues beat (triplet hi-hat)")
             ),
    UiAction("drum-pattern-triplet-fill",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Fill: triplets down the toms"),
             TranslatableString("action", "Fill the selected measures: fill: triplets down the toms")
             ),
    UiAction("drum-pattern-rock-8",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Rock beat (8th-note hi-hat)"),
             TranslatableString("action", "Fill the selected measures: rock beat (8th-note hi-hat)")
             ),
    UiAction("drum-pattern-rock-8-push",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Rock beat with pushed kick"),
             TranslatableString("action", "Fill the selected measures: rock beat with pushed kick")
             ),
    UiAction("drum-pattern-rock-16",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Rock beat (16th-note hi-hat)"),
             TranslatableString("action", "Fill the selected measures: rock beat (16th-note hi-hat)")
             ),
    UiAction("drum-pattern-quarter",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Rock beat (quarter-note hi-hat)"),
             TranslatableString("action", "Fill the selected measures: rock beat (quarter-note hi-hat)")
             ),
    UiAction("drum-pattern-four-floor",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Four on the floor"),
             TranslatableString("action", "Fill the selected measures: four on the floor")
             ),
    UiAction("drum-pattern-half-time",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Half-time beat"),
             TranslatableString("action", "Fill the selected measures: half-time beat")
             ),
    UiAction("drum-pattern-ride-8",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Rock beat on the ride"),
             TranslatableString("action", "Fill the selected measures: rock beat on the ride")
             ),
    UiAction("drum-pattern-crash-rock-8",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Rock beat starting with a crash"),
             TranslatableString("action", "Fill the selected measures: rock beat starting with a crash")
             ),
    UiAction("drum-pattern-snare-fill",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Fill: 16th notes on the snare"),
             TranslatableString("action", "Fill the selected measures: fill: 16th notes on the snare")
             ),
    UiAction("drum-pattern-tom-fill",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Fill: 16th notes down the toms"),
             TranslatableString("action", "Fill the selected measures: fill: 16th notes down the toms")
             ),
    UiAction("slash-fill",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Fill with &slashes"),
             TranslatableString("action", "Fill with slashes")
             ),
    UiAction("slash-rhythm",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle rhythmic sl&ash notation"),
             TranslatableString("action", "Toggle rhythmic slash notation")
             ),
    UiAction("pitch-spell",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Optimize enharmonic spelling"),
             TranslatableString("action", "Optimize enharmonic spelling")
             ),
    UiAction("pitch-spell-sharps",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Respell pitches with &sharps"),
             TranslatableString("action", "Respell pitches with sharps")
             ),
    UiAction("pitch-spell-flats",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Respell pitches with &flats"),
             TranslatableString("action", "Respell pitches with flats")
             ),
    UiAction("reset-groupings",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Regroup &rhythms"),
             TranslatableString("action", "Regroup rhythms")
             ),
    UiAction("resequence-rehearsal-marks",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Resequence re&hearsal marks"),
             TranslatableString("action", "Resequence rehearsal marks")
             ),
    UiAction("unroll-repeats",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "U&nroll repeats"),
             TranslatableString("action", "Unroll repeats")
             ),
    UiAction("drum-tab-paste",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Paste drum tab (text)"),
             TranslatableString("action", "Write the text drum tab on the clipboard into the score, from the selected measure on")
             ),
    UiAction("drum-tab-copy",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Copy as drum &tab (text)"),
             TranslatableString("action", "Copy the selected measures, or the whole score, as a text drum tab")
             ),
    UiAction("copy-lyrics-to-clipboard",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Copy &lyrics to clipboard"),
             TranslatableString("action", "Copy lyrics")
             ),
    UiAction("del-empty-measures",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Remove empty trailing meas&ures"),
             TranslatableString("action", "Remove empty trailing measures")
             ),
    UiAction("parts",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Parts"),
             TranslatableString("action", "Manage parts"),
             IconCode::Code::PAGE
             ),
    UiAction("view-mode-page",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Page view"),
             TranslatableString("action", "Display page view"),
             IconCode::Code::PAGE_VIEW
             ),
    UiAction("view-mode-float",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Floating"),
             TranslatableString("action", "Floating"),
             IconCode::Code::PAGE_VIEW // TODO, Icon needed?
             ),
    UiAction("view-mode-continuous",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Continuous view (horizontal)"),
             TranslatableString("action", "Display continuous view (horizontal)"),
             IconCode::Code::CONTINUOUS_VIEW
             ),
    UiAction("view-mode-single",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Continuous view (vertical)"),
             TranslatableString("action", "Display continuous view (vertical)"),
             IconCode::Code::CONTINUOUS_VIEW_VERTICAL
             ),
    UiAction("find",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Find / Go to"),
             TranslatableString("action", "Find / Go to")
             ),
    UiAction("staff-properties",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Staff/Part properties…"),
             TranslatableString("action", "Staff/Part properties")
             ),
    UiAction("staff-text-properties",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Staff text properties…"),
             TranslatableString("action", "Staff text properties")
             ),
    UiAction("system-text-properties",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "System text properties…"),
             TranslatableString("action", "System text properties")
             ),
    UiAction("measure-properties",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Measure properties…"),
             TranslatableString("action", "Measure properties")
             ),
    UiAction("measures-per-system",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Measures per s&ystem…"),
             TranslatableString("action", "Measures per system")
             ),
    UiAction("voice-x12",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Exchange voice &1-2"),
             TranslatableString("action", "Exchange voice 1-2")
             ),
    UiAction("voice-x13",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Exchange voice 1-3"),
             TranslatableString("action", "Exchange voice 1-3")
             ),
    UiAction("voice-x14",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Exchange voice 1-&4"),
             TranslatableString("action", "Exchange voice 1-4")
             ),
    UiAction("voice-x23",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Exchange voice &2-3"),
             TranslatableString("action", "Exchange voice 2-3")
             ),
    UiAction("voice-x24",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Exchange voice 2-4"),
             TranslatableString("action", "Exchange voice 2-4")
             ),
    UiAction("voice-x34",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Exchange voice &3-4"),
             TranslatableString("action", "Exchange voice 3-4")
             ),
    UiAction("system-break",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Add/remove system break"),
             TranslatableString("action", "Add/remove system break")
             ),
    UiAction("page-break",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add/remove page break"),
             TranslatableString("action", "Add/remove page break")
             ),
    UiAction("apply-system-lock",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Add/remove system lock"),
             TranslatableString("action", "Add/remove system lock")
             ),
    UiAction("move-measure-to-prev-system",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Move measure to previous system"),
             TranslatableString("action", "Move measure to previous system"),
             IconCode::Code::ARROW_UP
             ),
    UiAction("move-measure-to-next-system",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Move measure to next system"),
             TranslatableString("action", "Move measure to next system"),
             IconCode::Code::ARROW_DOWN
             ),
    UiAction("make-into-system",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Create system from selection"),
             TranslatableString("action", "Create system from selection")
             ),
    UiAction("section-break",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add/remove section break"),
             TranslatableString("action", "Add/remove section break")
             ),
    UiAction("split-measure",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Split measure before selected note/rest"),
             TranslatableString("action", "Split measure before selected note/rest")
             ),
    UiAction("join-measures",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Join selected measures"),
             TranslatableString("action", "Join selected measures")
             ),
    UiAction("insert-measure",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Insert one measure before selection"),
             TranslatableString("action", "Insert one measure before selection"),
             IconCode::Code::INSERT_ONE_MEASURE
             ),
    UiAction("insert-measures",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert &before selection…"),
             TranslatableString("action", "Insert measures before selection")
             ),
    UiAction("insert-measures-after-selection",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert &after selection…"),
             TranslatableString("action", "Insert measures after selection")
             ),
    UiAction("insert-measures-at-start-of-score",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert at &start of score…"),
             TranslatableString("action", "Insert measures at start of score")
             ),
    UiAction("append-measure",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert &one measure at end of score"),
             TranslatableString("action", "Insert one measure at end of score")
             ),
    UiAction("append-measures",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert at &end of score…"),
             TranslatableString("action", "Insert measures at end of score")
             ),
    UiAction("insert-hbox",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert &horizontal frame"),
             TranslatableString("action", "Insert horizontal frame"),
             IconCode::Code::HORIZONTAL_FRAME
             ),
    UiAction("insert-vbox",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert &vertical frame"),
             TranslatableString("action", "Insert vertical frame"),
             IconCode::Code::VERTICAL_FRAME
             ),
    UiAction("insert-textframe",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert &text frame"),
             TranslatableString("action", "Insert text frame"),
             IconCode::Code::TEXT_FRAME
             ),
    UiAction("insert-fretframe",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert &fretboard diagram legend"),
             TranslatableString("action", "Insert fretboard diagram legend"),
             IconCode::Code::FRET_FRAME
             ),
    UiAction("append-hbox",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Horizontal frame"),
             TranslatableString("action", "Insert horizontal frame at end of score"),
             IconCode::Code::HORIZONTAL_FRAME
             ),
    UiAction("append-vbox",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Vertical frame"),
             TranslatableString("action", "Insert vertical frame at end of score"),
             IconCode::Code::VERTICAL_FRAME
             ),
    UiAction("append-textframe",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Text frame"),
             TranslatableString("action", "Insert text frame at end of score"),
             IconCode::Code::TEXT_FRAME
             ),
    UiAction("append-fretframe",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Fretboard diagram legend"),
             TranslatableString("action", "Insert fretboard diagram legend at end of score"),
             IconCode::Code::FRET_FRAME
             ),
    UiAction("acciaccatura",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Acciaccatura"),
             TranslatableString("action", "Add grace note: acciaccatura"),
             IconCode::Code::ACCIACCATURA
             ),
    UiAction("appoggiatura",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Appoggiatura"),
             TranslatableString("action", "Add grace note: appoggiatura"),
             IconCode::Code::APPOGGIATURA
             ),
    UiAction("grace4",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Grace: quarter"),
             TranslatableString("action", "Add grace note: quarter"),
             IconCode::Code::GRACE4
             ),
    UiAction("grace16",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Grace: 16th"),
             TranslatableString("action", "Add grace note: 16th"),
             IconCode::Code::GRACE16
             ),
    UiAction("grace32",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Grace: 32nd"),
             TranslatableString("action", "Add grace note: 32nd"),
             IconCode::Code::GRACE32
             ),
    UiAction("grace8after",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Grace: 8th after"),
             TranslatableString("action", "Add grace note: eighth after"),
             IconCode::Code::GRACE8_AFTER
             ),
    UiAction("grace16after",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Grace: 16th after"),
             TranslatableString("action", "Add grace note: 16th after"),
             IconCode::Code::GRACE16_AFTER
             ),
    UiAction("grace32after",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Grace: 32nd after"),
             TranslatableString("action", "Add grace note: 32nd after"),
             IconCode::Code::GRACE32_AFTER
             ),
    UiAction("beam-auto",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Auto beam"),
             TranslatableString("action", "Auto beam"),
             IconCode::Code::AUTO_TEXT
             ),
    UiAction("beam-none",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "No beam"),
             TranslatableString("action", "No beam"),
             IconCode::Code::BEAM_NONE
             ),
    UiAction("beam-break-left",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Break beam left"),
             TranslatableString("action", "Break beam left"),
             IconCode::Code::BEAM_BREAK_LEFT
             ),
    UiAction("beam-break-inner-8th",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Break inner beams (8th)"),
             TranslatableString("action", "Break inner beams (eighth)"),
             IconCode::Code::BEAM_BREAK_INNER_8TH
             ),
    UiAction("beam-break-inner-16th",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Break inner beams (16th)"),
             TranslatableString("action", "Break inner beams (16th)"),
             IconCode::Code::BEAM_BREAK_INNER_16TH
             ),
    UiAction("beam-join",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Join beams"),
             TranslatableString("action", "Join beams"),
             IconCode::Code::BEAM_JOIN
             ),
    UiAction("beam-feathered-decelerate",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Feathered beam, decelerate"),
             TranslatableString("action", "Add feathered beam: decelerate"),
             IconCode::Code::BEAM_FEATHERED_DECELERATE
             ),
    UiAction("beam-feathered-accelerate",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Feathered beam, accelerate"),
             TranslatableString("action", "Add feathered beam: accelerate"),
             IconCode::Code::BEAM_FEATHERED_ACCELERATE
             ),
    UiAction("add-brackets",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add brackets to accidental"),
             TranslatableString("action", "Add brackets to accidental"),
             IconCode::Code::BRACKET_PARENTHESES_SQUARE
             ),
    UiAction("add-braces",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add braces to element"),
             TranslatableString("action", "Add braces to element"),
             IconCode::Code::BRACE
             ),
    UiAction("add-parentheses",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add parentheses to element"),
             TranslatableString("action", "Add parentheses to element"),
             IconCode::Code::BRACKET_PARENTHESES
             ),
    UiAction("interval1",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Unison"),
             TranslatableString("action", "Enter interval: unison")
             ),
    UiAction("interval2",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Se&cond above"),
             TranslatableString("action", "Enter interval: second above")
             ),
    UiAction("interval3",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Thir&d above"),
             TranslatableString("action", "Enter interval: third above")
             ),
    UiAction("interval4",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Fou&rth above"),
             TranslatableString("action", "Enter interval: fourth above")
             ),
    UiAction("interval5",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Fift&h above"),
             TranslatableString("action", "Enter interval: fifth above")
             ),
    UiAction("interval6",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Si&xth above"),
             TranslatableString("action", "Enter interval: sixth above")
             ),
    UiAction("interval7",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Seve&nth above"),
             TranslatableString("action", "Enter interval: seventh above")
             ),
    UiAction("interval8",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Octave &above"),
             TranslatableString("action", "Enter interval: octave above")
             ),
    UiAction("interval9",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Ninth abov&e"),
             TranslatableString("action", "Enter interval: ninth above")
             ),
    UiAction("interval10",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Tenth above"),
             TranslatableString("action", "Enter interval: tenth above")
             ),
    UiAction("interval-2",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Second below"),
             TranslatableString("action", "Enter interval: second below")
             ),
    UiAction("interval-3",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Third below"),
             TranslatableString("action", "Enter interval: third below")
             ),
    UiAction("interval-4",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "F&ourth below"),
             TranslatableString("action", "Enter interval: fourth below")
             ),
    UiAction("interval-5",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Fifth below"),
             TranslatableString("action", "Enter interval: fifth below")
             ),
    UiAction("interval-6",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "S&ixth below"),
             TranslatableString("action", "Enter interval: sixth below")
             ),
    UiAction("interval-7",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Se&venth below"),
             TranslatableString("action", "Enter interval: seventh below")
             ),
    UiAction("interval-8",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Octave &below"),
             TranslatableString("action", "Enter interval: octave below")
             ),
    UiAction("interval-9",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Ninth belo&w"),
             TranslatableString("action", "Enter interval: ninth below")
             ),
    UiAction("interval-10",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Tenth below"),
             TranslatableString("action", "Enter interval: tenth below")
             ),
    UiAction("note-c",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             noteC,
             Enter_note_X.arg(noteC)
             ),
    UiAction("note-d",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             noteD,
             Enter_note_X.arg(noteD)
             ),
    UiAction("note-e",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             noteE,
             Enter_note_X.arg(noteE)
             ),
    UiAction("note-f",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             noteF,
             Enter_note_X.arg(noteF)
             ),
    UiAction("note-g",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             noteG,
             Enter_note_X.arg(noteG)
             ),
    UiAction("note-a",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             noteA,
             Enter_note_X.arg(noteA)
             ),
    UiAction("note-b",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             noteB,
             Enter_note_X.arg(noteB)
             ),
    UiAction("chord-c",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Add_X_to_chord.arg(noteC),
             Add_note_X_to_chord.arg(noteC)
             ),
    UiAction("chord-d",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Add_X_to_chord.arg(noteD),
             Add_note_X_to_chord.arg(noteD)
             ),
    UiAction("chord-e",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Add_X_to_chord.arg(noteE),
             Add_note_X_to_chord.arg(noteE)
             ),
    UiAction("chord-f",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Add_X_to_chord.arg(noteF),
             Add_note_X_to_chord.arg(noteF)
             ),
    UiAction("chord-g",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Add_X_to_chord.arg(noteG),
             Add_note_X_to_chord.arg(noteG)
             ),
    UiAction("chord-a",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Add_X_to_chord.arg(noteA),
             Add_note_X_to_chord.arg(noteA)
             ),
    UiAction("chord-b",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Add_X_to_chord.arg(noteB),
             Add_note_X_to_chord.arg(noteB)
             ),
    UiAction("insert-c",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Insert_X.arg(noteC)
             ),
    UiAction("insert-d",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Insert_X.arg(noteD)
             ),
    UiAction("insert-e",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Insert_X.arg(noteE)
             ),
    UiAction("insert-f",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Insert_X.arg(noteF)
             ),
    UiAction("insert-g",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Insert_X.arg(noteG)
             ),
    UiAction("insert-a",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Insert_X.arg(noteA)
             ),
    UiAction("insert-b",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             Insert_X.arg(noteB)
             ),
    UiAction("rest",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "Rest"),
             TranslatableString("action", "Enter rest"),
             IconCode::Code::REST
             ),
    UiAction("rest-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             X_TAB.arg(TranslatableString("action", "Rest")),
             X_TAB.arg(TranslatableString("action", "Enter rest"))
             ),
    UiAction("fret-0",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(0),
             enter_TAB_fret_X.arg(0)
             ),
    UiAction("fret-1",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(1),
             enter_TAB_fret_X.arg(1)
             ),
    UiAction("fret-2",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(2),
             enter_TAB_fret_X.arg(2)
             ),
    UiAction("fret-3",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(3),
             enter_TAB_fret_X.arg(3)
             ),
    UiAction("fret-4",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(4),
             enter_TAB_fret_X.arg(4)
             ),
    UiAction("fret-5",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(5),
             enter_TAB_fret_X.arg(5)
             ),
    UiAction("fret-6",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(6),
             enter_TAB_fret_X.arg(6)
             ),
    UiAction("fret-7",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(7),
             enter_TAB_fret_X.arg(7)
             ),
    UiAction("fret-8",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(8),
             enter_TAB_fret_X.arg(8)
             ),
    UiAction("fret-9",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(9),
             enter_TAB_fret_X.arg(9)
             ),
    UiAction("fret-10",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(10),
             enter_TAB_fret_X.arg(10)
             ),
    UiAction("fret-11",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(11),
             enter_TAB_fret_X.arg(11)
             ),
    UiAction("fret-12",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(12),
             enter_TAB_fret_X.arg(12)
             ),
    UiAction("fret-13",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(13),
             enter_TAB_fret_X.arg(13)
             ),
    UiAction("fret-14",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             fret_X_TAB.arg(14),
             enter_TAB_fret_X.arg(14)
             ),
    UiAction("add-8va",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Ottava 8va &alta"),
             TranslatableString("action", "Add ottava 8va alta")
             ),
    UiAction("add-8vb",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Ottava 8va &bassa"),
             TranslatableString("action", "Add ottava 8va bassa")
             ),
    UiAction("add-dynamic",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Dynamic"),
             TranslatableString("action", "Add dynamic")
             ),
    UiAction("add-hairpin",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Crescendo"),
             TranslatableString("action", "Add hairpin: crescendo")
             ),
    UiAction("add-hairpin-reverse",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Diminuendo"),
             TranslatableString("action", "Add hairpin: diminuendo")
             ),
    UiAction("add-noteline",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Note-anchored line"),
             TranslatableString("action", "Add note-anchored line"),
             IconCode::Code::NOTE_ANCHORED_LINE
             ),
    UiAction("chord-tie",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add tied note to chord")
             ),
    UiAction("add-image",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Image")
             ),
    UiAction("title-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Title"),
             TranslatableString("action", "Add text: title")
             ),
    UiAction("subtitle-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Subtitle"),
             TranslatableString("action", "Add text: subtitle")
             ),
    UiAction("composer-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Composer"),
             TranslatableString("action", "Add text: composer")
             ),
    UiAction("poet-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Lyricist"),
             TranslatableString("action", "Add text: lyricist")
             ),
    UiAction("part-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Part name"),
             TranslatableString("action", "Add text: part name")
             ),
    UiAction("frame-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Text"),
             TranslatableString("action", "Add frame text")
             ),
    UiAction("system-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Syst&em text"),
             TranslatableString("action", "Add text: system text")
             ),
    UiAction("staff-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "St&aff text"),
             TranslatableString("action", "Add text: staff text")
             ),
    UiAction("expression-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "E&xpression text"),
             TranslatableString("action", "Add text: expression text")
             ),
    UiAction("rehearsalmark-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Rehearsal mark"),
             TranslatableString("action", "Add text: rehearsal mark")
             ),
    UiAction("instrument-change-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Instrument change"),
             TranslatableString("action", "Add text: instrument change")
             ),
    UiAction("fingering-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Fingering"),
             TranslatableString("action", "Add text: fingering")
             ),
    UiAction("sticking-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Stic&king"),
             TranslatableString("action", "Add text: sticking")
             ),
    UiAction("chord-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "C&hord symbol"),
             TranslatableString("action", "Add text: chord symbol")
             ),
    UiAction("roman-numeral-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "R&oman numeral analysis"),
             TranslatableString("action", "Add text: Roman numeral analysis")
             ),
    UiAction("nashville-number-text",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Nashville number"),
             TranslatableString("action", "Add text: Nashville number")
             ),
    UiAction("lyrics",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "L&yrics"),
             TranslatableString("action", "Add text: lyrics")
             ),
    UiAction("figured-bass",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Figured &bass"),
             TranslatableString("action", "Add text: figured bass")
             ),
    UiAction("tempo",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Tempo &marking"),
             TranslatableString("action", "Add text: tempo marking")
             ),
    UiAction("duplet",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "&Duplet"),
             TranslatableString("action", "Enter tuplet: duplet")
             ),
    UiAction("triplet",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "&Triplet"),
             TranslatableString("action", "Enter tuplet: triplet")
             ),
    UiAction("quadruplet",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "&Quadruplet"),
             TranslatableString("action", "Enter tuplet: quadruplet")
             ),
    UiAction("quintuplet",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Q&uintuplet"),
             TranslatableString("action", "Enter tuplet: quintuplet")
             ),
    UiAction("sextuplet",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Se&xtuplet"),
             TranslatableString("action", "Enter tuplet: sextuplet")
             ),
    UiAction("septuplet",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Se&ptuplet"),
             TranslatableString("action", "Enter tuplet: septuplet")
             ),
    UiAction("octuplet",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "&Octuplet"),
             TranslatableString("action", "Enter tuplet: octuplet")
             ),
    UiAction("nonuplet",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "&Nonuplet"),
             TranslatableString("action", "Enter tuplet: nonuplet")
             ),
    UiAction("tuplet-dialog",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Othe&r…"),
             TranslatableString("action", "Enter tuplet: other")
             ),
    UiAction("stretch-",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Decrease layout stretch"),
             TranslatableString("action", "Decrease layout stretch")
             ),
    UiAction("stretch+",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Increase layout stretch"),
             TranslatableString("action", "Increase layout stretch")
             ),
    UiAction("reset-stretch",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "&Reset layout stretch"),
             TranslatableString("action", "Reset layout stretch")
             ),
    UiAction("reset-text-style-overrides",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Reset &text style overrides"),
             TranslatableString("action", "Reset all text style overrides to default")
             ),
    UiAction("reset-beammode",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Reset &beams"),
             TranslatableString("action", "Reset beams to default grouping")
             ),
    UiAction("reset",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Reset s&hapes and positions"),
             TranslatableString("action", "Reset shapes and positions")
             ),
    UiAction("reset-to-default-layout",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Reset entire score to &default layout"),
             TranslatableString("action", "Reset entire score to default layout")
             ),
    UiAction("zoomin",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Zoom in"),
             TranslatableString("action", "Zoom in"),
             IconCode::Code::ZOOM_IN
             ),
    UiAction("zoomout",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Zoom out"),
             TranslatableString("action", "Zoom out"),
             IconCode::Code::ZOOM_OUT
             ),
    UiAction("zoom100",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Zoom to 100%"),
             TranslatableString("action", "Zoom to 100%")
             ),
    UiAction("zoom-page-width",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Zoom to page width"),
             TranslatableString("action", "Zoom to page width")
             ),
    UiAction("zoom-whole-page",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Zoom to whole page"),
             TranslatableString("action", "Zoom to whole page")
             ),
    UiAction("zoom-two-pages",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Zoom to two pages"),
             TranslatableString("action", "Zoom to two pages")
             ),
    UiAction("get-location",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Accessibility: Get location"),
             TranslatableString("action", "Accessibility: get location")
             ),
    UiAction("edit-element",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Edit element"),
             TranslatableString("action", "Edit element")
             ),
    UiAction("select-prev-measure",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Select to beginning of measure"),
             TranslatableString("action", "Select to beginning of measure")
             ),
    UiAction("select-next-measure",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Select to end of measure"),
             TranslatableString("action", "Select to end of measure")
             ),
    UiAction("select-begin-line",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Select to beginning of line"),
             TranslatableString("action", "Select to beginning of line")
             ),
    UiAction("select-end-line",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Select to end of line"),
             TranslatableString("action", "Select to end of line")
             ),
    UiAction("select-begin-score",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Select to beginning of score"),
             TranslatableString("action", "Select to beginning of score")
             ),
    UiAction("select-end-score",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Select to end of score"),
             TranslatableString("action", "Select to end of score")
             ),
    UiAction("select-staff-above",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add staff above to selection"),
             TranslatableString("action", "Add to selection: staff above")
             ),
    UiAction("select-staff-below",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add staff below to selection"),
             TranslatableString("action", "Add to selection: staff below")
             ),
    UiAction("scr-prev",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Screen: Previous"),
             TranslatableString("action", "Jump to previous screen")
             ),
    UiAction("scr-next",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Screen: Next"),
             TranslatableString("action", "Jump to next screen")
             ),
    UiAction("page-prev",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Page: Previous"),
             TranslatableString("action", "Jump to previous page")
             ),
    UiAction("page-next",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Page: Next"),
             TranslatableString("action", "Jump to next page")
             ),
    UiAction("page-top",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Page: Top of first"),
             TranslatableString("action", "Jump to top of first page")
             ),
    UiAction("page-end",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Page: Bottom of last"),
             TranslatableString("action", "Jump to bottom of last page")
             ),
    UiAction("help",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Context sensitive help")
             ),
    UiAction("repeat-sel",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Repeat selection"),
             TranslatableString("action", "Repeat selection")
             ),
    UiAction("toggle-score-lock",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Lock/unlock all systems"),
             TranslatableString("action", "Lock/unlock all systems")
             ),
    UiAction("toggle-system-lock",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Lock/unlock selected system(s)"),
             TranslatableString("action", "Lock/unlock selected system(s)"),
             IconCode::Code::SYSTEM_LOCK
             ),
    UiAction("enh-both",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Change enharmonic spelling (concert and transposed &pitch)"),
             TranslatableString("action", "Change enharmonic spelling (concert and transposed pitch)"),
             IconCode::Code::NONE
             ),
    UiAction("enh-current",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Change enharmonic spelling (&current pitch mode only)"),
             TranslatableString("action", "Change enharmonic spelling (current pitch mode only)"),
             IconCode::Code::NONE
             ),
    UiAction("flip",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Flip direction"),
             TranslatableString("action", "Flip direction"),
             IconCode::Code::NOTE_FLIP
             ),
    UiAction("flip-horizontally",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Flip horizontally"),
             TranslatableString("action", "Flip horizontally")),
    UiAction(TOGGLE_CONCERT_PITCH_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Concert pitch"),
             TranslatableString("action", "Toggle concert pitch"),
             IconCode::Code::TUNING_FORK,
             Checkable::Yes
             ),
    UiAction("next-word",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Next word"),
             TranslatableString("action", "Go to next word")
             ),
    UiAction("next-text-element",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Next text element"),
             TranslatableString("action", "Go to next text element")
             ),
    UiAction("prev-text-element",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Previous text element"),
             TranslatableString("action", "Go to previous text element")
             ),
    UiAction("next-beat-TEXT",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: next beat (chord symbols)"),
             TranslatableString("action", "Advance cursor: next beat (chord symbols)")
             ),
    UiAction("prev-beat-TEXT",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: previous beat (chord symbols)"),
             TranslatableString("action", "Advance cursor: previous beat (chord symbols)")
             ),
    UiAction("advance-longa",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: longa (chord symbols/figured bass)"),
             TranslatableString("action", "Advance cursor: longa (chord symbols/figured bass)")
             ),
    UiAction("advance-breve",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: breve (chord symbols/figured bass)"),
             TranslatableString("action", "Advance cursor: breve (chord symbols/figured bass)")
             ),
    UiAction("advance-1",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: whole note (chord symbols/figured bass)"),
             TranslatableString("action", "Advance cursor: whole note (chord symbols/figured bass)")
             ),
    UiAction("advance-2",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: half note (chord symbols/figured bass)"),
             TranslatableString("action", "Advance cursor: half note (chord symbols/figured bass)")
             ),
    UiAction("advance-4",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: quarter note (chord symbols/figured bass)"),
             TranslatableString("action", "Advance cursor: quarter note (chord symbols/figured bass)")
             ),
    UiAction("advance-8",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: eighth note (chord symbols/figured bass)"),
             TranslatableString("action", "Advance cursor: eighth note (chord symbols/figured bass)")
             ),
    UiAction("advance-16",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: 16th note (chord symbols/figured bass)"),
             TranslatableString("action", "Advance cursor: 16th note (chord symbols/figured bass)")
             ),
    UiAction("advance-32",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: 32nd note (chord symbols/figured bass)"),
             TranslatableString("action", "Advance cursor: 32nd note (chord symbols/figured bass)")
             ),
    UiAction("advance-64",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Advance cursor: 64th note (chord symbols/figured bass)"),
             TranslatableString("action", "Advance cursor: 64th note (chord symbols/figured bass)")
             ),
    UiAction("next-lyric-verse",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Next lyric verse"),
             TranslatableString("action", "Move text/go to next lyric verse")
             ),
    UiAction("prev-lyric-verse",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Previous lyric verse"),
             TranslatableString("action", "Move text/go to previous lyric verse")
             ),
    UiAction("next-syllable",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Next syllable"),
             TranslatableString("action", "Lyrics: enter hyphen")
             ),
    UiAction("add-melisma",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Add extension line"),
             TranslatableString("action", "Lyrics: enter extension line")
             ),
    UiAction("add-lyric-verse",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add lyrics verse"),
             TranslatableString("action", "Add lyrics verse")
             ),
    UiAction("text-b",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Bold face"),
             TranslatableString("action", "Format text: bold"),
             Checkable::Yes
             ),
    UiAction("text-i",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Italic"),
             TranslatableString("action", "Format text: italic"),
             Checkable::Yes
             ),
    UiAction("text-u",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Underline"),
             TranslatableString("action", "Format text: underline"),
             Checkable::Yes
             ),
    UiAction("text-s",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Strikethrough"),
             TranslatableString("action", "Format text: strikethrough"),
             Checkable::Yes
             ),
    UiAction("text-sub",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Subscript"),
             TranslatableString("action", "Format text: subscript"),
             Checkable::Yes
             ),
    UiAction("text-sup",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_TEXT_EDITING,
             TranslatableString("action", "Superscript"),
             TranslatableString("action", "Format text: superscript"),
             Checkable::Yes
             ),
    UiAction("pitch-up-diatonic",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Diatonic pitch up"),
             TranslatableString("action", "Move pitch up diatonically")
             ),
    UiAction("pitch-down-diatonic",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Diatonic pitch down"),
             TranslatableString("action", "Move pitch down diatonically")
             ),
    UiAction("top-staff",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_FOCUSED,
             TranslatableString("action", "Go to top staff"),
             TranslatableString("action", "Go to top staff")
             ),
    UiAction("empty-trailing-measure",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Go to first empty trailing measure"),
             TranslatableString("action", "Go to first empty trailing measure")
             ),
    UiAction("mirror-note",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Mirror notehead"),
             TranslatableString("action", "Mirror notehead")
             ),
    UiAction("add-turn",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle turn"),
             TranslatableString("action", "Add ornament: turn")
             ),
    UiAction("add-turn-inverted",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle inverted turn"),
             TranslatableString("action", "Add ornament: inverted turn")
             ),
    UiAction("add-turn-slash",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle turn with slash"),
             TranslatableString("action", "Add ornament: turn with slash")
             ),
    UiAction("add-turn-up",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle vertical turn"),
             TranslatableString("action", "Add ornament: vertical turn")
             ),
    UiAction("add-turn-inverted-up",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle vertical inverted turn"),
             TranslatableString("action", "Add ornament: vertical inverted turn")
             ),
    UiAction("add-trill",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle trill"),
             TranslatableString("action", "Add ornament: trill")
             ),
    UiAction("add-short-trill",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle short trill"),
             TranslatableString("action", "Add ornament: short trill")
             ),
    UiAction("add-mordent",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle mordent"),
             TranslatableString("action", "Add ornament: mordent")
             ),
    UiAction("add-haydn",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle Haydn ornament"),
             TranslatableString("action", "Add ornament: Haydn ornament")
             ),
    UiAction("add-tremblement",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle tremblement"),
             TranslatableString("action", "Add ornament: tremblement")
             ),
    UiAction("add-prall-mordent",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle prall mordent"),
             TranslatableString("action", "Add ornament: prall mordent")
             ),
    UiAction("add-shake",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle shake"),
             TranslatableString("action", "Add ornament: shake")
             ),
    UiAction("add-shake-muffat",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle shake (Muffat)"),
             TranslatableString("action", "Add ornament: shake (Muffat)")
             ),
    UiAction("add-tremblement-couperin",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle tremblement appuyé (Couperin)"),
             TranslatableString("action", "Add ornament: tremblement appuyé (Couperin)")
             ),
    UiAction("add-up-bow",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle up bow"),
             TranslatableString("action", "Add bowing: up bow")
             ),
    UiAction("add-down-bow",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle down bow"),
             TranslatableString("action", "Add bowing: down bow")
             ),
    UiAction("reset-style",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Reset style"),
             TranslatableString("action", "Reset all style values to default")
             ),
    UiAction("clef-violin",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add treble clef"),
             TranslatableString("action", "Add clef: treble")
             ),
    UiAction("clef-bass",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add bass clef"),
             TranslatableString("action", "Add clef: bass")
             ),
    UiAction("sharp2-post",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add double-sharp"),
             TranslatableString("action", "Add accidental: double-sharp")
             ),
    UiAction("sharp-post",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add sharp"),
             TranslatableString("action", "Add accidental: sharp")
             ),
    UiAction("nat-post",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add natural"),
             TranslatableString("action", "Add accidental: natural")
             ),
    UiAction("flat-post",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add flat"),
             TranslatableString("action", "Add accidental: flat")
             ),
    UiAction("flat2-post",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Add double-flat"),
             TranslatableString("action", "Add accidental: double-flat")
             ),
    UiAction("transpose-up",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Transpose up"),
             TranslatableString("action", "Transpose up a semitone")
             ),
    UiAction("transpose-down",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Transpose down"),
             TranslatableString("action", "Transpose down a semitone")
             ),
    UiAction("pitch-up-diatonic-alterations",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Diatonic pitch up (keep degree alterations)"),
             TranslatableString("action", "Move pitch up diatonically (keep degree alterations)")
             ),
    UiAction("pitch-down-diatonic-alterations",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Diatonic pitch down (keep degree alterations)"),
             TranslatableString("action", "Move pitch down diatonically (keep degree alterations)")
             ),
    UiAction("full-measure-rest",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Full measure rest"),
             TranslatableString("action", "Insert full measure rest")
             ),
    UiAction("toggle-mmrest",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle multimeasure rests"),
             TranslatableString("action", "Toggle multimeasure rests")
             ),
    UiAction("toggle-hide-empty",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle empty staves"),
             TranslatableString("action", "Show/hide empty staves")
             ),
    UiAction("set-visible",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Set visible"),
             TranslatableString("action", "Make selected element(s) visible")
             ),
    UiAction("unset-visible",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Set invisible"),
             TranslatableString("action", "Make selected element(s) invisible")
             ),
    UiAction("toggle-autoplace",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle automatic placement for selected elements"),
             TranslatableString("action", "Toggle automatic placement for selected elements")
             ),
    UiAction("autoplace-enabled",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle automatic placement for entire score"),
             TranslatableString("action", "Toggle automatic placement for entire score")
             ),
    UiAction("string-above",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "String above (TAB)"),
             TranslatableString("action", "Go to string above (TAB)")
             ),
    UiAction("string-below",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "String below (TAB)"),
             TranslatableString("action", "Go to string below (TAB)")
             ),
    UiAction(NOTE_INPUT_ACTION_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Note input"),
             TranslatableString("action", "Toggle note input mode"),
             IconCode::Code::EDIT,
             Checkable::Yes
             ),
    UiAction("toggle-insert-mode",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert/overwrite"),
             TranslatableString("action", "Toggle note input mode: insert/overwrite")
             ),
    UiAction("note-input-by-note-name",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Input by note name"),
             TranslatableString("action", "Toggle note input mode: input by note name"),
             IconCode::Code::EDIT
             ),
    UiAction("note-input-by-duration",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Input by duration"),
             TranslatableString("action", "Toggle note input mode: input by duration"),
             IconCode::Code::DURATION_CURSOR
             ),
    UiAction("note-input-rhythm",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Rhythm only (not pitch)"),
             TranslatableString("action", "Toggle note input mode: rhythm only (not pitch)"),
             IconCode::Code::RHYTHM_ONLY
             ),
    UiAction("note-input-repitch",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Re-pitch existing notes"),
             TranslatableString("action", "Toggle note input mode: re-pitch existing notes"),
             IconCode::Code::RE_PITCH
             ),
    UiAction("note-input-realtime-auto",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Real-time (metronome)"),
             TranslatableString("action", "Toggle note input mode: real-time (metronome)"),
             IconCode::Code::METRONOME
             ),
    UiAction("note-input-realtime-manual",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Real-time (foot pedal)"),
             TranslatableString("action", "Toggle note input mode: real-time (foot pedal)"),
             IconCode::Code::FOOT_PEDAL
             ),
    UiAction("note-input-timewise",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Insert"),
             TranslatableString("action", "Toggle note input mode: insert (increases measure duration)"),
             IconCode::Code::NOTE_PLUS
             ),
    UiAction("realtime-advance",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Real-time advance"),
             TranslatableString("action", "Real-time advance"),
             IconCode::Code::METRONOME
             ),
    UiAction("note-longa",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "Longa"),
             TranslatableString("action", "Set duration: longa"),
             IconCode::Code::LONGO
             ),
    UiAction("note-breve",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "Double whole note"),
             TranslatableString("action", "Set duration: double whole note"),
             IconCode::Code::NOTE_WHOLE_DOUBLE
             ),
    UiAction("pad-note-1",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "Whole note"),
             TranslatableString("action", "Set duration: whole note"),
             IconCode::Code::NOTE_WHOLE
             ),
    UiAction("pad-note-2",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "Half note"),
             TranslatableString("action", "Set duration: half note"),
             IconCode::Code::NOTE_HALF
             ),
    UiAction("pad-note-4",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "Quarter note"),
             TranslatableString("action", "Set duration: quarter note"),
             IconCode::Code::NOTE_QUARTER
             ),
    UiAction("pad-note-8",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "Eighth note"),
             TranslatableString("action", "Set duration: eighth note"),
             IconCode::Code::NOTE_8TH
             ),
    UiAction("pad-note-16",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "16th note"),
             TranslatableString("action", "Set duration: 16th note"),
             IconCode::Code::NOTE_16TH
             ),
    UiAction("pad-note-32",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "32nd note"),
             TranslatableString("action", "Set duration: 32nd note"),
             IconCode::Code::NOTE_32ND
             ),
    UiAction("pad-note-64",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "64th note"),
             TranslatableString("action", "Set duration: 64th note"),
             IconCode::Code::NOTE_64TH
             ),
    UiAction("pad-note-128",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "128th note"),
             TranslatableString("action", "Set duration: 128th note"),
             IconCode::Code::NOTE_128TH
             ),
    UiAction("pad-note-256",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "256th note"),
             TranslatableString("action", "Set duration: 256th note"),
             IconCode::Code::NOTE_256TH
             ),
    UiAction("pad-note-512",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "512th note"),
             TranslatableString("action", "Set duration: 512th note"),
             IconCode::Code::NOTE_512TH
             ),
    UiAction("pad-note-1024",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_NOT_NOTE_INPUT_STAFF_TAB,
             TranslatableString("action", "1024th note"),
             TranslatableString("action", "Set duration: 1024th note"),
             IconCode::Code::NOTE_1024TH
             ),
    UiAction("pad-note-1-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "Whole note")),
             X_TAB.arg(TranslatableString("action", "Set duration: whole note")),
             IconCode::Code::NOTE_WHOLE
             ),
    UiAction("pad-note-2-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "Half note")),
             X_TAB.arg(TranslatableString("action", "Set duration: half note")),
             IconCode::Code::NOTE_HALF
             ),
    UiAction("pad-note-4-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "Quarter note")),
             X_TAB.arg(TranslatableString("action", "Set duration: quarter note")),
             IconCode::Code::NOTE_QUARTER
             ),
    UiAction("pad-note-8-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "Eighth note")),
             X_TAB.arg(TranslatableString("action", "Set duration: eighth note")),
             IconCode::Code::NOTE_8TH
             ),
    UiAction("pad-note-16-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "16th note")),
             X_TAB.arg(TranslatableString("action", "Set duration: 16th note")),
             IconCode::Code::NOTE_16TH
             ),
    UiAction("pad-note-32-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "32nd note")),
             X_TAB.arg(TranslatableString("action", "Set duration: 32nd note")),
             IconCode::Code::NOTE_32ND
             ),
    UiAction("pad-note-64-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "64th note")),
             X_TAB.arg(TranslatableString("action", "Set duration: 64th note")),
             IconCode::Code::NOTE_64TH
             ),
    UiAction("pad-note-128-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "128th note")),
             X_TAB.arg(TranslatableString("action", "Set duration: 128th note")),
             IconCode::Code::NOTE_128TH
             ),
    UiAction("pad-note-256-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "256th note")),
             X_TAB.arg(TranslatableString("action", "Set duration: 256th note")),
             IconCode::Code::NOTE_256TH
             ),
    UiAction("pad-note-512-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "512th note")),
             X_TAB.arg(TranslatableString("action", "Set duration: 512th note")),
             IconCode::Code::NOTE_512TH
             ),
    UiAction("pad-note-1024-TAB",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             X_TAB.arg(TranslatableString("action", "1024th note")),
             X_TAB.arg(TranslatableString("action", "Set duration: 1024th note")),
             IconCode::Code::NOTE_1024TH
             ),
    UiAction("pad-dot",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Augmentation dot"),
             TranslatableString("action", "Toggle duration dot"),
             IconCode::Code::NOTE_DOTTED
             ),
    UiAction("pad-dot2",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Double augmentation dot"),
             TranslatableString("action", "Toggle duration dot: double"),
             IconCode::Code::NOTE_DOTTED_2
             ),
    UiAction("pad-dot3",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Triple augmentation dot"),
             TranslatableString("action", "Toggle duration dot: triple"),
             IconCode::Code::NOTE_DOTTED_3
             ),
    UiAction("pad-dot4",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Quadruple augmentation dot"),
             TranslatableString("action", "Toggle duration dot: quadruple"),
             IconCode::Code::NOTE_DOTTED_4
             ),
    UiAction("pad-rest",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Rest"),
             TranslatableString("action", "Toggle rest"),
             IconCode::Code::REST
             ),
    UiAction("next-segment-element",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Accessibility: Next segment element"),
             TranslatableString("action", "Select next in-staff element")
             ),
    UiAction("prev-segment-element",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Accessibility: Previous segment element"),
             TranslatableString("action", "Select previous in-staff element")
             ),
    UiAction("flat",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle flat"),
             TranslatableString("action", "Toggle accidental: flat"),
             IconCode::Code::FLAT
             ),
    UiAction("flat2",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle double-flat"),
             TranslatableString("action", "Toggle accidental: double-flat"),
             IconCode::Code::FLAT_DOUBLE
             ),
    UiAction("nat",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle natural"),
             TranslatableString("action", "Toggle accidental: natural"),
             IconCode::Code::NATURAL
             ),
    UiAction("sharp",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle sharp"),
             TranslatableString("action", "Toggle accidental: sharp"),
             IconCode::Code::SHARP
             ),
    UiAction("sharp2",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Toggle double-sharp"),
             TranslatableString("action", "Toggle accidental: double-sharp"),
             IconCode::Code::SHARP_DOUBLE
             ),
    UiAction("tie",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Tie"),
             TranslatableString("action", "Add tied note"),
             IconCode::Code::NOTE_TIE
             ),
    UiAction("lv",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Laissez vibrer"),
             TranslatableString("action", "Add laissez vibrer"),
             IconCode::Code::NOTE_LV
             ),
    UiAction("add-slur",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Slur"),
             TranslatableString("action", "Add slur"),
             IconCode::Code::NOTE_SLUR
             ),
    UiAction("add-marcato",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Marcato"),
             TranslatableString("action", "Add articulation: marcato"),
             IconCode::Code::MARCATO
             ),
    UiAction("add-sforzato",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Accent"),
             TranslatableString("action", "Add articulation: accent"),
             IconCode::Code::ACCENT
             ),
    UiAction("add-tenuto",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Tenuto"),
             TranslatableString("action", "Add articulation: tenuto"),
             IconCode::Code::TENUTO
             ),
    UiAction("add-staccato",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Staccato"),
             TranslatableString("action", "Add articulation: staccato"),
             IconCode::Code::STACCATO
             ),
    UiAction("cross-staff-beaming",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Cross-staff beaming"),
             IconCode::Code::CROSS_STAFF_BEAMING
             ),
    UiAction("tuplet",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Tuplet"),
             IconCode::Code::NOTE_TUPLET
             ),
    UiAction("voice-1",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Voice 1"),
             TranslatableString("action", "Use voice 1"),
             IconCode::Code::VOICE_1
             ),
    UiAction("voice-2",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Voice 2"),
             TranslatableString("action", "Use voice 2"),
             IconCode::Code::VOICE_2
             ),
    UiAction("voice-3",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Voice 3"),
             TranslatableString("action", "Use voice 3"),
             IconCode::Code::VOICE_3
             ),
    UiAction("voice-4",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Voice 4"),
             TranslatableString("action", "Use voice 4"),
             IconCode::Code::VOICE_4
             ),
    UiAction("voice-assignment-all-in-instrument",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "All voices on instrument"),
             TranslatableString("action", "Use all voices on instrument")
             ),
    UiAction("voice-assignment-all-in-staff",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "All voices on staff"),
             TranslatableString("action", "Use all voices on staff")
             ),
    UiAction("notation-context-menu",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_FOCUSED
             ),
    UiAction("insert-staff-type-change",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Staff type change"),
             TranslatableString("action", "Insert staff type change"),
             IconCode::Code::STAFF_TYPE_CHANGE
             ),
    UiAction("notation-popup-menu",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_FOCUSED
             ),
    UiAction("standard-bend",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Standard bend"),
             TranslatableString("action", "Standard bend"),
             IconCode::Code::GUITAR_BEND_REGULAR
             ),
    UiAction("pre-bend",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Pre-bend"),
             TranslatableString("action", "Pre-bend"),
             IconCode::Code::GUITAR_PRE_BEND
             ),
    UiAction("grace-note-bend",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Grace note bend"),
             TranslatableString("action", "Grace note bend"),
             IconCode::Code::GUITAR_GRACE_NOTE_BEND
             ),
    UiAction("slight-bend",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Slight bend"),
             TranslatableString("action", "Slight bend"),
             IconCode::Code::GUITAR_SLIGHT_BEND
             ),
    UiAction("dive",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             //: Standard guitar dive, i.e. a movement of the tremolo bar between two pitches
             TranslatableString("action", "Dive"),
             TranslatableString("action", "Dive"),
             IconCode::Code::GUITAR_DIVE_REGULAR
             ),
    UiAction("pre-dive",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             //: Pre-dive, i.e. a movement of the tremolo bar prepared before picking the note
             TranslatableString("action", "Pre-dive"),
             TranslatableString("action", "Pre-dive"),
             IconCode::Code::GUITAR_PRE_DIVE
             ),
    UiAction("dip",
             //: Dip, i.e. a quick touch of the tremolo bar after the note is picked
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Dip"),
             TranslatableString("action", "Dip"),
             IconCode::Code::GUITAR_DIP_DOWN
             ),
    UiAction("scoop",
             //: Scoop, i.e. a quick movement of the tremolo bar at the start of the note
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Scoop"),
             TranslatableString("action", "Scoop"),
             IconCode::Code::GUITAR_SCOOP
             ),
    UiAction("hammer-on-pull-off",
             kn::context::UiCtxProjectFocused,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Hammer-on/pull-off"),
             TranslatableString("action", "Add hammer-on/pull-off")
             )
};

const UiActionList NotationUiActions::s_scoreConfigActions = {
    UiAction(SHOW_INVISIBLE_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show &invisible"),
             TranslatableString("action", "Show/hide invisible elements"),
             Checkable::Yes
             ),
    UiAction(SHOW_UNPRINTABLE_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show f&ormatting"),
             TranslatableString("action", "Show/hide formatting"),
             Checkable::Yes
             ),
    UiAction(SHOW_FRAMES_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show &frames"),
             TranslatableString("action", "Show/hide frames"),
             Checkable::Yes
             ),
    UiAction(SHOW_PAGEBORDERS_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show page &margins"),
             TranslatableString("action", "Show/hide page margins"),
             Checkable::Yes
             ),
    UiAction(SHOW_SOUND_FLAGS,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show sound flags"), // todo &
             TranslatableString("action", "Show/hide sound flags"),
             Checkable::Yes
             ),
    UiAction(SHOW_IRREGULAR_CODE,
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Mark i&rregular measures"),
             TranslatableString("action", "Mark irregular measures"),
             Checkable::Yes
             )
};

const UiActionList NotationUiActions::s_undoRedoActions = {
    UiAction("action://notation/undo",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_DISABLED,
             TranslatableString("action", "Undo"),
             TranslatableString("action", "Undo"),
             IconCode::Code::UNDO
             ),
    UiAction("action://notation/redo",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_DISABLED,
             TranslatableString("action", "Redo"),
             TranslatableString("action", "Redo"),
             IconCode::Code::REDO
             )
};

const UiActionList NotationUiActions::s_engravingDebuggingActions = {
    UiAction("show-element-bounding-rects",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show element bounding rectangles"),
             TranslatableString("action", "Show/hide element bounding rectangles"),
             Checkable::Yes
             ),
    UiAction("color-element-shapes",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Color element shapes"),
             Checkable::Yes
             ),
    UiAction("show-segment-shapes",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show segment shapes"),
             TranslatableString("action", "Show/hide segment shapes"),
             Checkable::Yes
             ),
    UiAction("color-segment-shapes",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Color segment shapes"),
             Checkable::Yes
             ),
    UiAction("show-skylines",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show skylines"),
             TranslatableString("action", "Show/hide skylines"),
             Checkable::Yes
             ),
    UiAction("show-system-bounding-rects",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show system bounding rectangles"),
             TranslatableString("action", "Show/hide system bounding rectangles"),
             Checkable::Yes
             ),
    UiAction("show-element-masks",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show element masks"),
             TranslatableString("action", "Show/hide element masks"),
             Checkable::Yes
             ),
    UiAction("show-line-attach-points",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show line-attach points"),
             TranslatableString("action", "Show/hide line-attach points"),
             Checkable::Yes
             ),
    UiAction("mark-empty-staff-visibility-overrides",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Mark empty staff visibility overrides"),
             TranslatableString("action", "Mark empty staff visibility overrides"),
             Checkable::Yes
             ),
    UiAction("mark-corrupted-measures",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Mark corrupted measures"),
             TranslatableString("action", "Mark corrupted measures"),
             Checkable::Yes
             ),
    UiAction("show-gap-rests",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Show gap rests"),
             TranslatableString("action", "Show gap rests"),
             Checkable::Yes
             ),
    UiAction("check-for-score-corruptions",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED,
             TranslatableString("action", "Check for score corruptions")
             ),
    UiAction("edit-strings",
             kn::context::UiCtxProjectOpened,
             kn::context::CTX_NOTATION_OPENED
             )
};

NotationUiActions::NotationUiActions(std::shared_ptr<NotationActionController> controller, const kfw::modularity::ContextPtr& iocCtx)
    : kfw::Contextable(iocCtx), m_controller(controller)
{
}

void NotationUiActions::init()
{
    m_controller->currentNotationNoteInputChanged().onNotify(this, [this]() {
        m_actionCheckedChanged.send({ NOTE_INPUT_ACTION_CODE });
    });

    m_controller->currentNotationChanged().onNotify(this, [this]() {
        ActionCodeList actions;
        actions.reserve(s_scoreConfigActions.size());
        for (const UiAction& action : s_scoreConfigActions) {
            actions.push_back(action.code);
        }
        m_actionCheckedChanged.send(actions);

        m_actionEnabledMap.reserve(actionsList().size());
        updateActionsEnabled(actionsList());

        const INotationInteractionPtr interaction = m_controller->currentNotationInteraction();

        if (interaction) {
            interaction->selectionChanged().onNotify(this, [this]() {
                updateActionsEnabled(s_actions);
            }, Asyncable::Mode::SetReplace);

            interaction->isEditingElementChanged().onNotify(this, [this]() {
                updateActionsEnabled(s_actions);
            }, Asyncable::Mode::SetReplace);

            interaction->noteInput()->noteInputStarted().onReceive(this, [this](bool) {
                updateActionsEnabled(s_actions);
            }, Asyncable::Mode::SetReplace);

            interaction->noteInput()->noteInputEnded().onNotify(this, [this]() {
                updateActionsEnabled(s_actions);
            }, Asyncable::Mode::SetReplace);

            m_controller->currentNotationUndoStack()->stackChanged().onNotify(this, [this]() {
                updateActionsEnabled(s_undoRedoActions);
                // KitNote: undo and redo change the style without the style saying so
                m_actionCheckedChanged.send({ SWING_OFF_CODE, SWING_EIGHTH_CODE, SWING_SIXTEENTH_CODE });
            }, Asyncable::Mode::SetReplace);

            interaction->scoreConfigChanged().onReceive(this, [this](ScoreConfigType configType) {
                static const std::unordered_map<ScoreConfigType, std::string> configActions {
                    { ScoreConfigType::ShowInvisibleElements, SHOW_INVISIBLE_CODE },
                    { ScoreConfigType::ShowUnprintableElements, SHOW_UNPRINTABLE_CODE },
                    { ScoreConfigType::ShowFrames, SHOW_FRAMES_CODE },
                    { ScoreConfigType::ShowPageMargins, SHOW_PAGEBORDERS_CODE },
                    { ScoreConfigType::MarkIrregularMeasures, SHOW_IRREGULAR_CODE },
                    { ScoreConfigType::ShowSoundFlags, SHOW_SOUND_FLAGS },
                };

                m_actionCheckedChanged.send({ configActions.at(configType) });
            }, Asyncable::Mode::SetReplace);
        }

        m_actionCheckedChanged.send({ TOGGLE_CONCERT_PITCH_CODE, SWING_OFF_CODE, SWING_EIGHTH_CODE, SWING_SIXTEENTH_CODE });
        m_controller->currentNotationStyleChanged().onNotify(this, [this]() {
            m_actionCheckedChanged.send({ TOGGLE_CONCERT_PITCH_CODE, SWING_OFF_CODE, SWING_EIGHTH_CODE, SWING_SIXTEENTH_CODE });
        }, Asyncable::Mode::SetReplace);
    });

    engravingConfiguration()->debuggingOptionsChanged().onNotify(this, [this]() {
        ActionCodeList actions;
        actions.reserve(s_engravingDebuggingActions.size());
        for (const UiAction& action : s_engravingDebuggingActions) {
            actions.push_back(action.code);
        }
        m_actionCheckedChanged.send(actions);
    });
}

const UiActionList& NotationUiActions::actionsList() const
{
    static UiActionList alist;
    if (alist.empty()) {
        alist.insert(alist.end(), s_actions.begin(), s_actions.end());
        alist.insert(alist.end(), s_undoRedoActions.begin(), s_undoRedoActions.end());
        alist.insert(alist.end(), s_scoreConfigActions.begin(), s_scoreConfigActions.end());
        alist.insert(alist.end(), s_engravingDebuggingActions.begin(), s_engravingDebuggingActions.end());
    }
    return alist;
}

bool NotationUiActions::actionEnabled(const UiAction& act) const
{
    return m_controller->canReceiveAction(act.code);
}

void NotationUiActions::updateActionsEnabled(const UiActionList& actions)
{
    TRACEFUNC;

    ActionCodeList codes;

    for (const UiAction& action : actions) {
        const bool enabled = m_controller->canReceiveAction(action.code);

        auto it = m_actionEnabledMap.find(action.code);
        if (it == m_actionEnabledMap.end()) {
            m_actionEnabledMap.insert({ action.code, enabled });
            codes.push_back(action.code);
            continue;
        }

        if (it->second != enabled) {
            it->second = enabled;
            codes.push_back(action.code);
        }
    }

    if (!codes.empty()) {
        m_actionEnabledChanged.send(codes);
    }
}

bool NotationUiActions::isScoreConfigAction(const ActionCode& code) const
{
    for (const UiAction& a : s_scoreConfigActions) {
        if (a.code == code) {
            return true;
        }
    }
    return false;
}

bool NotationUiActions::isScoreConfigChecked(const ActionCode& code, const ScoreConfig& cfg) const
{
    if (SHOW_INVISIBLE_CODE == code) {
        return cfg.isShowInvisibleElements;
    }
    if (SHOW_UNPRINTABLE_CODE == code) {
        return cfg.isShowUnprintableElements;
    }
    if (SHOW_FRAMES_CODE == code) {
        return cfg.isShowFrames;
    }
    if (SHOW_PAGEBORDERS_CODE == code) {
        return cfg.isShowPageMargins;
    }
    if (SHOW_SOUND_FLAGS == code) {
        return cfg.isShowSoundFlags;
    }
    if (SHOW_IRREGULAR_CODE == code) {
        return cfg.isMarkIrregularMeasures;
    }

    return false;
}

bool NotationUiActions::actionChecked(const UiAction& act) const
{
    if (act.code == NOTE_INPUT_ACTION_CODE) {
        auto noteInput = m_controller->currentNotationNoteInput();
        if (noteInput) {
            return noteInput->isNoteInputMode();
        }
    }

    if (act.code == TOGGLE_CONCERT_PITCH_CODE) {
        auto style = m_controller->currentNotationStyle();
        if (style) {
            return style->styleValue(StyleId::concertPitch).toBool();
        }
    }

    // KitNote: the swing of the score
    if (act.code == SWING_OFF_CODE || act.code == SWING_EIGHTH_CODE || act.code == SWING_SIXTEENTH_CODE) {
        auto style = m_controller->currentNotationStyle();
        if (style) {
            const kfw::ByteArray ba = style->styleValue(StyleId::swingUnit).value<String>().toAscii();
            const kn::engraving::DurationType unit = kn::engraving::TConv::fromXml(ba.constChar(),
                                                                                   kn::engraving::DurationType::V_INVALID);
            if (act.code == SWING_EIGHTH_CODE) {
                return unit == kn::engraving::DurationType::V_EIGHTH;
            }
            if (act.code == SWING_SIXTEENTH_CODE) {
                return unit == kn::engraving::DurationType::V_16TH;
            }
            return unit != kn::engraving::DurationType::V_EIGHTH && unit != kn::engraving::DurationType::V_16TH;
        }
    }

    if (isScoreConfigAction(act.code)) {
        auto interaction = m_controller->currentNotationInteraction();
        if (interaction) {
            return isScoreConfigChecked(act.code, interaction->scoreConfig());
        }
    }

    auto engravingDebuggingActionsSearch = NotationActionController::engravingDebuggingActions.find(act.code);
    if (engravingDebuggingActionsSearch != NotationActionController::engravingDebuggingActions.cend()) {
        return engravingConfiguration()->debuggingOptions().*(engravingDebuggingActionsSearch->second);
    }

    return false;
}

kfw::async::Channel<ActionCodeList> NotationUiActions::actionEnabledChanged() const
{
    return m_actionEnabledChanged;
}

kfw::async::Channel<ActionCodeList> NotationUiActions::actionCheckedChanged() const
{
    return m_actionCheckedChanged;
}

DurationType NotationUiActions::actionDurationType(const ActionCode& actionCode)
{
    static const QMap<ActionCode, DurationType> durations = {
        { "note-longa", DurationType::V_LONG },
        { "note-breve", DurationType::V_BREVE },
        { "pad-note-1", DurationType::V_WHOLE },
        { "pad-note-2", DurationType::V_HALF },
        { "pad-note-4", DurationType::V_QUARTER },
        { "pad-note-8", DurationType::V_EIGHTH },
        { "pad-note-16", DurationType::V_16TH },
        { "pad-note-32", DurationType::V_32ND },
        { "pad-note-64", DurationType::V_64TH },
        { "pad-note-128", DurationType::V_128TH },
        { "pad-note-256", DurationType::V_256TH },
        { "pad-note-512", DurationType::V_512TH },
        { "pad-note-1024", DurationType::V_1024TH }
    };

    DurationType type = DurationType::V_INVALID;
    if (durations.contains(actionCode)) {
        type = durations[actionCode];
    }

    return type;
}

AccidentalType NotationUiActions::actionAccidentalType(const ActionCode& actionCode)
{
    static const QMap<ActionCode, AccidentalType> accidentals = {
        { "flat2", AccidentalType::FLAT2 },
        { "flat", AccidentalType::FLAT },
        { "nat", AccidentalType::NATURAL },
        { "sharp", AccidentalType::SHARP },
        { "sharp2", AccidentalType::SHARP2 }
    };

    AccidentalType type = AccidentalType::NONE;
    if (accidentals.contains(actionCode)) {
        type = accidentals[actionCode];
    }

    return type;
}

int NotationUiActions::actionDotCount(const ActionCode& actionCode)
{
    static const QMap<ActionCode, int> dots = {
        { "pad-dot", 1 },
        { "pad-dot2", 2 },
        { "pad-dot3", 3 },
        { "pad-dot4", 4 }
    };

    int dotCount = 0;
    if (dots.contains(actionCode)) {
        dotCount = dots[actionCode];
    }

    return dotCount;
}

int NotationUiActions::actionVoice(const ActionCode& actionCode)
{
    static const QMap<ActionCode, int> voices {
        { "voice-1", 0 },
        { "voice-2", 1 },
        { "voice-3", 2 },
        { "voice-4", 3 }
    };

    int voice = 0;
    if (voices.contains(actionCode)) {
        voice = voices[actionCode];
    }

    return voice;
}

SymbolId NotationUiActions::actionArticulationSymbolId(const ActionCode& actionCode)
{
    static const QMap<ActionCode, SymbolId> articulations {
        { "add-marcato", SymbolId::articMarcatoAbove },
        { "add-sforzato", SymbolId::articAccentAbove },
        { "add-tenuto", SymbolId::articTenutoAbove },
        { "add-staccato", SymbolId::articStaccatoAbove }
    };

    SymbolId symbolId = SymbolId::noSym;
    if (articulations.contains(actionCode)) {
        symbolId = articulations[actionCode];
    }

    return symbolId;
}

const kfw::ui::ToolConfig& NotationUiActions::defaultNoteInputBarConfig()
{
    static ToolConfig config;
    if (!config.isValid()) {
        config.items = {
            { "note-input-by-note-name", true },
            { "note-input-by-duration", true },
            { "note-input-rhythm", false },
            { "note-input-repitch", false },
            { "note-input-realtime-auto", false },
            { "note-input-realtime-manual", false },
            { "note-input-timewise", false },
            { "", true },
            { "pad-note-1024", false },
            { "pad-note-512", false },
            { "pad-note-256", false },
            { "pad-note-128", false },
            { "pad-note-64", true },
            { "pad-note-32", true },
            { "pad-note-16", true },
            { "pad-note-8", true },
            { "pad-note-4", true },
            { "pad-note-2", true },
            { "pad-note-1", true },
            { "note-breve", false },
            { "note-longa", false },
            { "", true },
            { "pad-dot", true },
            { "pad-dot2", false },
            { "pad-dot3", false },
            { "pad-dot4", false },
            { "pad-rest", true },
            { "", true },
            // KitNote: accidentals, ties and slurs have no use on a drum staff.
            // They stay available from "Customize toolbar".
            { "flat2", false },
            { "flat", false },
            { "nat", false },
            { "sharp", false },
            { "sharp2", false },
            { "", true },
            { "tie", false },
            { "add-slur", false },
            { "lv", false },
            { "", true },
            { "add-marcato", true },
            { "add-sforzato", true },
            { "add-tenuto", true },
            { "add-staccato", true },
            { "", true },
            // KitNote: ghost notes (parentheses) and flams (acciaccatura) one click away.
            { "add-parentheses", true },
            { "acciaccatura", true },
            { "", true },
            { "cross-staff-beaming", false },
            { "tuplet", true },
            { "flip", true },
            { "", true },
            { "voice-1", true },
            { "voice-2", true },
            { "voice-3", false },
            { "voice-4", false }
        };
    }
    return config;
}
