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
#include "notationactioncontroller.h"

#include "engraving/types/typesconv.h" // KitNote

#include "io/file.h"

#include "engraving/dom/masterscore.h"
#include "engraving/dom/note.h"
#include "engraving/dom/text.h"
#include "engraving/dom/sig.h"
#include "notation/notationtypes.h"

#include "qml/KitNote/NotationScene/abstractelementpopupmodel.h"

#include "translation.h"
#include "log.h"

#include <QGuiApplication>

using namespace kn;
using namespace kfw;
using namespace kfw::io;
using namespace kfw::ui;
using namespace kn::notation;
using namespace kfw::actions;
using namespace kn::context;

static constexpr qreal STRETCH_STEP = 0.1;
static constexpr bool NEAR_NOTE_OR_REST = true;

static constexpr bool DONT_PLAY_CHORD = false;

static const ActionCode UNDO_ACTION_CODE = "action://notation/undo";
static const ActionCode REDO_ACTION_CODE = "action://notation/redo";

static const QMap<ActionCode, Fraction> DURATIONS_FOR_TEXT_NAVIGATION {
    { "advance-longa", Fraction(4, 1) },
    { "advance-breve", Fraction(2, 1) },
    { "advance-1", Fraction(1, 1) },
    { "advance-2", Fraction(1, 2) },
    { "advance-4", Fraction(1, 4) },
    { "advance-8", Fraction(1, 8) },
    { "advance-16", Fraction(1, 16) },
    { "advance-32", Fraction(1, 32) },
    { "advance-64", Fraction(1, 64) },
};

using EngravingDebuggingOptions = NotationActionController::EngravingDebuggingOptions;
const std::unordered_map<ActionCode, bool EngravingDebuggingOptions::*> NotationActionController::engravingDebuggingActions {
    { "show-element-bounding-rects", &EngravingDebuggingOptions::showElementBoundingRects },
    { "color-element-shapes", &EngravingDebuggingOptions::colorElementShapes },
    { "show-segment-shapes", &EngravingDebuggingOptions::showSegmentShapes },
    { "color-segment-shapes", &EngravingDebuggingOptions::colorSegmentShapes },
    { "show-skylines", &EngravingDebuggingOptions::showSkylines },
    { "show-system-bounding-rects", &EngravingDebuggingOptions::showSystemBoundingRects },
    { "show-element-masks", &EngravingDebuggingOptions::showElementMasks },
    { "show-line-attach-points", &EngravingDebuggingOptions::showLineAttachPoints },
    { "mark-empty-staff-visibility-overrides", &EngravingDebuggingOptions::markEmptyStaffVisibilityOverrides },
    { "mark-corrupted-measures", &EngravingDebuggingOptions::markCorruptedMeasures },
    { "show-gap-rests", &EngravingDebuggingOptions::showGapRests }
};

//! NOTE Just for more readable
using Controller = NotationActionController;
using Interaction = INotationInteraction;

void NotationActionController::init()
{
    TRACEFUNC;

    //! NOTE For historical reasons, the name of the action does not match what needs to be done
    registerAction("action://notation/cancel", &Controller::resetState, &Controller::isNotationPage);

    registerAction("note-input", &Controller::toggleNoteInput, &Controller::startNoteInputAllowed);
    registerNoteInputAction("note-input-by-note-name", NoteInputMethod::BY_NOTE_NAME);
    registerNoteInputAction("note-input-by-duration", NoteInputMethod::BY_DURATION);
    registerNoteInputAction("note-input-rhythm", NoteInputMethod::RHYTHM);
    registerNoteInputAction("note-input-repitch", NoteInputMethod::REPITCH);
    registerNoteInputAction("note-input-realtime-auto", NoteInputMethod::REALTIME_AUTO);
    registerNoteInputAction("note-input-realtime-manual", NoteInputMethod::REALTIME_MANUAL);
    registerNoteInputAction("note-input-timewise", NoteInputMethod::TIMEWISE);

    registerAction("realtime-advance", &Controller::realtimeAdvance, &Controller::isNoteInputMode);

    registerPadNoteAction("note-longa", Pad::NOTE00);
    registerPadNoteAction("note-breve", Pad::NOTE0);
    registerPadNoteAction("pad-note-1", Pad::NOTE1);
    registerPadNoteAction("pad-note-2", Pad::NOTE2);
    registerPadNoteAction("pad-note-4", Pad::NOTE4);
    registerPadNoteAction("pad-note-8", Pad::NOTE8);
    registerPadNoteAction("pad-note-16", Pad::NOTE16);
    registerPadNoteAction("pad-note-32", Pad::NOTE32);
    registerPadNoteAction("pad-note-64", Pad::NOTE64);
    registerPadNoteAction("pad-note-128", Pad::NOTE128);
    registerPadNoteAction("pad-note-256", Pad::NOTE256);
    registerPadNoteAction("pad-note-512", Pad::NOTE512);
    registerPadNoteAction("pad-note-1024", Pad::NOTE1024);
    registerPadNoteAction("pad-dot", Pad::DOT);
    registerPadNoteAction("pad-dot2", Pad::DOT2);
    registerPadNoteAction("pad-dot3", Pad::DOT3);
    registerPadNoteAction("pad-dot4", Pad::DOT4);
    registerPadNoteAction("pad-rest", Pad::REST);

    registerAction("note-action", &Controller::handleNoteAction);

    registerNoteAction("note-c", NoteName::C);
    registerNoteAction("note-d", NoteName::D);
    registerNoteAction("note-e", NoteName::E);
    registerNoteAction("note-f", NoteName::F);
    registerNoteAction("note-g", NoteName::G);
    registerNoteAction("note-a", NoteName::A);
    registerNoteAction("note-b", NoteName::B);

    registerNoteAction("chord-c", NoteName::C, NoteAddingMode::CurrentChord);
    registerNoteAction("chord-d", NoteName::D, NoteAddingMode::CurrentChord);
    registerNoteAction("chord-e", NoteName::E, NoteAddingMode::CurrentChord);
    registerNoteAction("chord-f", NoteName::F, NoteAddingMode::CurrentChord);
    registerNoteAction("chord-g", NoteName::G, NoteAddingMode::CurrentChord);
    registerNoteAction("chord-a", NoteName::A, NoteAddingMode::CurrentChord);
    registerNoteAction("chord-b", NoteName::B, NoteAddingMode::CurrentChord);

    registerNoteAction("insert-c", NoteName::C, NoteAddingMode::InsertChord);
    registerNoteAction("insert-d", NoteName::D, NoteAddingMode::InsertChord);
    registerNoteAction("insert-e", NoteName::E, NoteAddingMode::InsertChord);
    registerNoteAction("insert-f", NoteName::F, NoteAddingMode::InsertChord);
    registerNoteAction("insert-g", NoteName::G, NoteAddingMode::InsertChord);
    registerNoteAction("insert-a", NoteName::A, NoteAddingMode::InsertChord);
    registerNoteAction("insert-b", NoteName::B, NoteAddingMode::InsertChord);

    registerAction("next-text-element", &Controller::nextTextElement, &Controller::textNavigationAvailable);
    registerAction("prev-text-element", &Controller::prevTextElement, &Controller::textNavigationAvailable);
    registerAction("next-word", &Controller::nextWord, &Controller::textNavigationAvailable);
    registerAction("next-beat-TEXT", &Controller::nextBeatTextElement, &Controller::textNavigationByBeatsAvailable);
    registerAction("prev-beat-TEXT", &Controller::prevBeatTextElement, &Controller::textNavigationByBeatsAvailable);

    for (auto it = DURATIONS_FOR_TEXT_NAVIGATION.cbegin(); it != DURATIONS_FOR_TEXT_NAVIGATION.cend(); ++it) {
        registerAction(it.key(), [=]() { navigateToTextElementByFraction(it.value()); }, &Controller::textNavigationByFractionAvailable);
    }

    registerAction("next-lyric-verse", &Interaction::navigateToLyricsVerse, MoveDirection::Down, PlayMode::NoPlay,
                   &Controller::isEditingLyrics);
    registerAction("prev-lyric-verse", &Interaction::navigateToLyricsVerse, MoveDirection::Up, PlayMode::NoPlay,
                   &Controller::isEditingLyrics);
    registerAction("next-syllable", &Interaction::navigateToNextSyllable, PlayMode::NoPlay, &Controller::isEditingLyrics);

    registerAction("add-melisma", &Interaction::addMelisma, PlayMode::NoPlay, &Controller::isEditingLyrics);
    registerAction("add-lyric-verse", &Interaction::addLyricsVerse, PlayMode::NoPlay, &Controller::isEditingLyrics);

    registerAction("flat2", [this]() { toggleAccidental(AccidentalType::FLAT2); });
    registerAction("flat", [this]() { toggleAccidental(AccidentalType::FLAT); });
    registerAction("nat", [this]() { toggleAccidental(AccidentalType::NATURAL); });
    registerAction("sharp", [this]() { toggleAccidental(AccidentalType::SHARP); });
    registerAction("sharp2", [this]() { toggleAccidental(AccidentalType::SHARP2); });

    registerAction("rest", &Interaction::putRestToSelection);

    registerAction("add-marcato", [this]() { toggleArticulation(SymbolId::articMarcatoAbove); });
    registerAction("add-sforzato", [this]() { toggleArticulation(SymbolId::articAccentAbove); });
    registerAction("add-tenuto", [this]() { toggleArticulation(SymbolId::articTenutoAbove); });
    registerAction("add-staccato", [this]() { toggleArticulation(SymbolId::articStaccatoAbove); });

    registerAction("duplet", [this]() { putTuplet(2); }, &Controller::noteOrRestSelected);
    registerAction("triplet", [this]() { putTuplet(3); }, &Controller::noteOrRestSelected);
    registerAction("quadruplet", [this]() { putTuplet(4); }, &Controller::noteOrRestSelected);
    registerAction("quintuplet", [this]() { putTuplet(5); }, &Controller::noteOrRestSelected);
    registerAction("sextuplet", [this]() { putTuplet(6); }, &Controller::noteOrRestSelected);
    registerAction("septuplet", [this]() { putTuplet(7); }, &Controller::noteOrRestSelected);
    registerAction("octuplet", [this]() { putTuplet(8); }, &Controller::noteOrRestSelected);
    registerAction("nonuplet", [this]() { putTuplet(9); }, &Controller::noteOrRestSelected);
    registerAction("custom-tuplet", &Controller::putTuplet, &Controller::noteOrRestSelected);
    registerAction("tuplet-dialog", &Controller::openTupletOtherDialog, &Controller::noteOrRestSelected);

    registerAction("put-note", &Controller::putNote);
    registerAction("remove-note", &Controller::removeNote);

    registerAction("toggle-visible", &Interaction::toggleVisible, &Controller::isToggleVisibleAllowed);

    registerMoveSelectionAction("next-element", MoveSelectionType::EngravingItem, MoveDirection::Right, PlayMode::PlayNote);
    registerMoveSelectionAction("prev-element", MoveSelectionType::EngravingItem, MoveDirection::Left, PlayMode::PlayNote);
    registerMoveSelectionAction("next-track", MoveSelectionType::Track, MoveDirection::Right, PlayMode::PlayChord);
    registerMoveSelectionAction("prev-track", MoveSelectionType::Track, MoveDirection::Left, PlayMode::PlayChord);
    registerMoveSelectionAction("next-frame", MoveSelectionType::Frame, MoveDirection::Right);
    registerMoveSelectionAction("prev-frame", MoveSelectionType::Frame, MoveDirection::Left);
    registerMoveSelectionAction("next-system", MoveSelectionType::System, MoveDirection::Right);
    registerMoveSelectionAction("prev-system", MoveSelectionType::System, MoveDirection::Left);

    registerAction("notation-move-right", &Controller::move, MoveDirection::Right, false, &Controller::isNotEditingOrHasPopup);
    registerAction("notation-move-left", &Controller::move, MoveDirection::Left, false, &Controller::isNotEditingOrHasPopup);
    registerAction("notation-move-right-quickly", &Controller::move, MoveDirection::Right, true, &Controller::measureNavigationAvailable);
    registerAction("notation-move-left-quickly", &Controller::move, MoveDirection::Left, true, &Controller::measureNavigationAvailable);
    registerAction("pitch-up", &Controller::move, MoveDirection::Up, false, &Controller::isNotEditingOrHasPopup);
    registerAction("pitch-down", &Controller::move, MoveDirection::Down, false, &Controller::isNotEditingOrHasPopup);
    registerAction("pitch-up-octave", &Controller::move, MoveDirection::Up, true, &Controller::isNotEditingOrHasPopup);
    registerAction("pitch-down-octave", &Controller::move, MoveDirection::Down, true, &Controller::isNotEditingOrHasPopup);
    registerAction("up-chord", [this]() { moveWithinChord(MoveDirection::Up); }, &Controller::hasSelection);
    registerAction("down-chord", [this]() { moveWithinChord(MoveDirection::Down); }, &Controller::hasSelection);

    registerAction("double-duration", &Controller::doubleNoteInputDuration);
    registerAction("half-duration", &Controller::halveNoteInputDuration);
    registerAction("inc-duration-dotted", &Interaction::increaseDecreaseDuration, -1, true);
    registerAction("dec-duration-dotted", &Interaction::increaseDecreaseDuration, 1, true);

    registerAction("action://notation/copy", &Interaction::copySelection, &Controller::hasSelection);
    registerAction("action://notation/cut", &Controller::cutSelection, &Controller::hasSelection);
    registerAction("action://notation/paste", [this]() { pasteSelection(PastingType::Default); }, &Controller::isNotationPage);
    registerAction("notation-paste-half", [this]() { pasteSelection(PastingType::Half); });
    registerAction("notation-paste-double", [this]() { pasteSelection(PastingType::Double); });
    registerAction("notation-paste-special", [this]() { pasteSelection(PastingType::Special); });
    registerAction("notation-swap", &Interaction::swapSelection, &Controller::hasSelection);
    registerAction("action://notation/delete", &Interaction::deleteSelection, &Controller::hasSelection);

    registerAction("flip", &Interaction::flipSelection, &Controller::hasSelection);
    registerAction("flip-horizontally", &Interaction::flipSelectionHorizontally, &Controller::hasSelection);
    registerAction("tie", &Controller::addTie);
    registerAction("chord-tie", &Controller::chordTie);
    registerAction("lv", &Controller::addLaissezVib);
    registerAction("add-slur", &Controller::addSlur);
    registerAction("hammer-on-pull-off", &Controller::addHammerOnPullOff);

    registerAction(UNDO_ACTION_CODE, &Interaction::undo, &Controller::canUndo);
    registerAction(REDO_ACTION_CODE, &Interaction::redo, &Controller::canRedo);

    registerAction("select-next-chord", &Interaction::addToSelection, MoveDirection::Right, MoveSelectionType::Chord, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("select-prev-chord", &Interaction::addToSelection, MoveDirection::Left, MoveSelectionType::Chord, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("select-similar", &Controller::selectAllSimilarElements, &Controller::hasSelection);
    registerAction("select-similar-staff", &Controller::selectAllSimilarElementsInStaff, &Controller::hasSelection);
    registerAction("select-similar-range", &Controller::selectAllSimilarElementsInRange, &Controller::hasSelection);
    registerAction("select-dialog", &Controller::openSelectionMoreOptions, &Controller::hasSelection);
    registerAction("notation-select-all", &Interaction::selectAll);
    registerAction("notation-select-section", &Interaction::selectSection);
    registerAction("first-element", &Interaction::selectFirstElement, false, PlayMode::PlayChord);
    registerAction("last-element", &Interaction::selectLastElement, PlayMode::PlayChord);
    registerAction("top-chord", [this]() { selectTopOrBottomOfChord(MoveDirection::Up); }, &Controller::hasSelection);
    registerAction("bottom-chord", [this]() { selectTopOrBottomOfChord(MoveDirection::Down); }, &Controller::hasSelection);
    registerAction("move-up", &Interaction::moveChordRestToStaff, MoveDirection::Up, &Controller::hasSelection);
    registerAction("move-down", &Interaction::moveChordRestToStaff, MoveDirection::Down, &Controller::hasSelection);
    registerAction("move-left", &Interaction::swapChordRest, MoveDirection::Left, &Controller::isNoteInputMode);
    registerAction("move-right", &Interaction::swapChordRest, MoveDirection::Right, &Controller::isNoteInputMode);
    registerAction("toggle-snap-to-previous", &Interaction::toggleSnapToPrevious, &Controller::hasSelection);
    registerAction("toggle-snap-to-next", &Interaction::toggleSnapToNext, &Controller::hasSelection);
    registerAction("next-segment-element", &Interaction::moveSegmentSelection, MoveDirection::Right, PlayMode::PlayNote);
    registerAction("prev-segment-element", &Interaction::moveSegmentSelection, MoveDirection::Left, PlayMode::PlayNote);

    registerAction("system-break", &Interaction::toggleLayoutBreak, LayoutBreakType::LINE, PlayMode::NoPlay,
                   &Controller::toggleLayoutBreakAvailable);
    registerAction("page-break", &Interaction::toggleLayoutBreak, LayoutBreakType::PAGE, PlayMode::NoPlay,
                   &Controller::toggleLayoutBreakAvailable);
    registerAction("section-break", &Interaction::toggleLayoutBreak, LayoutBreakType::SECTION, PlayMode::NoPlay,
                   &Controller::toggleLayoutBreakAvailable);

    registerAction("apply-system-lock", &Interaction::applySystemLock);
    registerAction("move-measure-to-prev-system", &Interaction::moveMeasureToPrevSystem);
    registerAction("move-measure-to-next-system", &Interaction::moveMeasureToNextSystem);
    registerAction("toggle-system-lock", &Interaction::toggleSystemLock);
    registerAction("toggle-score-lock", &Interaction::toggleScoreLock);
    registerAction("make-into-system", &Interaction::makeIntoSystem);

    registerAction("split-measure", &Interaction::splitSelectedMeasure);
    registerAction("join-measures", &Interaction::joinSelectedMeasures);

    registerAction("insert-measure", [this]() {
        addBoxes(BoxType::Measure, 1, AddBoxesTarget::BeforeSelection);
    }, &Controller::hasSelection);
    registerAction("insert-measures", [this](const ActionData& actionData) {
        addMeasures(actionData, AddBoxesTarget::BeforeSelection);
    }, &Controller::hasSelection);
    registerAction("insert-measures-after-selection", [this](const ActionData& actionData) {
        addMeasures(actionData, AddBoxesTarget::AfterSelection);
    }, &Controller::hasSelection);
    registerAction("insert-measures-at-start-of-score", [this](const ActionData& actionData) {
        addMeasures(actionData, AddBoxesTarget::AtStartOfScore);
    });
    registerAction("append-measure", [this]() {
        addBoxes(BoxType::Measure, 1, AddBoxesTarget::AtEndOfScore);
    });
    registerAction("append-measures", [this](const ActionData& actionData) {
        addMeasures(actionData, AddBoxesTarget::AtEndOfScore);
    });

    registerAction("insert-hbox", [this]() { addBoxes(BoxType::Horizontal, 1, AddBoxesTarget::BeforeSelection); });
    registerAction("insert-vbox", [this]() { addBoxes(BoxType::Vertical, 1, AddBoxesTarget::BeforeSelection); });
    registerAction("insert-textframe", [this]() { addBoxes(BoxType::Text, 1, AddBoxesTarget::BeforeSelection); });
    registerAction("insert-fretframe", [this]() { addBoxes(BoxType::Fret, 1, AddBoxesTarget::BeforeSelection); });
    registerAction("append-hbox", [this]() { addBoxes(BoxType::Horizontal, 1, AddBoxesTarget::AtEndOfScore); });
    registerAction("append-vbox", [this]() { addBoxes(BoxType::Vertical, 1, AddBoxesTarget::AtEndOfScore); });
    registerAction("append-textframe", [this]() { addBoxes(BoxType::Text, 1, AddBoxesTarget::AtEndOfScore); });
    registerAction("append-fretframe", [this]() { addBoxes(BoxType::Fret, 1, AddBoxesTarget::AtEndOfScore); });

    registerAction("edit-style", &Controller::openEditStyleDialog);
    registerAction("page-settings", &Controller::openPageSettingsDialog);
    registerAction("staff-properties", &Controller::openStaffProperties);
    registerAction("edit-strings", &Controller::openEditStringsDialog);
    registerAction("measures-per-system", &Controller::openBreaksDialog);
    registerAction("transpose", &Controller::openTransposeDialog);
    registerAction("parts", &Controller::openPartsDialog);
    registerAction("staff-text-properties", &Controller::openStaffTextPropertiesDialog);
    registerAction("system-text-properties", &Controller::openStaffTextPropertiesDialog);
    registerAction("measure-properties", &Controller::openMeasurePropertiesDialog);
    registerAction("config-raster", &Controller::openEditGridSizeDialog);
    registerAction("realize-chord-symbols", &Controller::openRealizeChordSymbolsDialog);
    registerAction("add-fretboard-diagram", &Controller::addFretboardDiagram);

    registerAction("load-style", &Controller::loadStyle);
    registerAction("save-style", &Controller::saveStyle);

    registerAction("voice-x12", &Interaction::swapVoices, 0, 1);
    registerAction("voice-x13", &Interaction::swapVoices, 0, 2);
    registerAction("voice-x14", &Interaction::swapVoices, 0, 3);
    registerAction("voice-x23", &Interaction::swapVoices, 1, 2);
    registerAction("voice-x24", &Interaction::swapVoices, 1, 3);
    registerAction("voice-x34", &Interaction::swapVoices, 2, 3);

    registerAction("add-8va", &Interaction::addOttavaToSelection, OttavaType::OTTAVA_8VA);
    registerAction("add-8vb", &Interaction::addOttavaToSelection, OttavaType::OTTAVA_8VB);
    registerAction("add-dynamic", &Interaction::toggleDynamicPopup, &Controller::noteOrRestSelected);
    registerAction("add-hairpin", &Interaction::addHairpinsToSelection, HairpinType::CRESC_HAIRPIN, &Controller::noteOrRestSelected);
    registerAction("add-hairpin-reverse", &Interaction::addHairpinsToSelection, HairpinType::DIM_HAIRPIN,
                   &Controller::noteOrRestSelected);
    registerAction("add-noteline", &Interaction::addAnchoredLineToSelectedNotes);

    registerAction("add-image", [this]() { addImage(); });

    registerAction("title-text", [this]() { addText(TextStyleType::TITLE); });
    registerAction("subtitle-text", [this]() { addText(TextStyleType::SUBTITLE); });
    registerAction("composer-text", [this]() { addText(TextStyleType::COMPOSER); });
    registerAction("poet-text", [this]() { addText(TextStyleType::LYRICIST); });
    registerAction("part-text", [this]() { addText(TextStyleType::INSTRUMENT_EXCERPT); });
    registerAction("frame-text", [this]() { addText(TextStyleType::FRAME); });

    registerAction("system-text", [this]() { addText(TextStyleType::SYSTEM); });
    registerAction("staff-text", [this]() { addText(TextStyleType::STAFF); });
    registerAction("expression-text", [this]() { addText(TextStyleType::EXPRESSION); });
    registerAction("rehearsalmark-text", [this]() { addText(TextStyleType::REHEARSAL_MARK); });
    registerAction("instrument-change-text", [this]() { addText(TextStyleType::INSTRUMENT_CHANGE); });
    registerAction("fingering-text", [this]() { addText(TextStyleType::FINGERING); });
    registerAction("sticking-text", [this]() { addText(TextStyleType::STICKING); });
    registerAction("chord-text", [this]() { addText(TextStyleType::HARMONY_A); });
    registerAction("roman-numeral-text", [this]() { addText(TextStyleType::HARMONY_ROMAN); });
    registerAction("nashville-number-text", [this]() { addText(TextStyleType::HARMONY_NASHVILLE); });
    registerAction("lyrics", [this]() { addText(TextStyleType::LYRICS_ODD); });
    registerAction("tempo", [this]() { addText(TextStyleType::TEMPO); });

    registerAction("figured-bass", [this]() { addFiguredBass(); });

    registerAction("stretch-", [this]() { addStretch(-STRETCH_STEP); });
    registerAction("stretch+", [this]() { addStretch(STRETCH_STEP); });

    registerAction("reset-stretch", &Controller::resetStretch);
    registerAction("reset-text-style-overrides", &Interaction::resetTextStyleOverrides);
    registerAction("reset-beammode", &Controller::resetBeamMode);
    registerAction("reset", &Interaction::resetShapesAndPosition, &Controller::hasSelection);
    registerAction("reset-to-default-layout", &Interaction::resetToDefaultLayout);

    registerAction("show-invisible", [this]() { toggleScoreConfig(ScoreConfigType::ShowInvisibleElements); });
    registerAction("show-unprintable", [this]() { toggleScoreConfig(ScoreConfigType::ShowUnprintableElements); });
    registerAction("show-frames", [this]() { toggleScoreConfig(ScoreConfigType::ShowFrames); });
    registerAction("show-pageborders", [this]() { toggleScoreConfig(ScoreConfigType::ShowPageMargins); });
    registerAction("show-soundflags", [this]() { toggleScoreConfig(ScoreConfigType::ShowSoundFlags); });
    registerAction("show-irregular", [this]() { toggleScoreConfig(ScoreConfigType::MarkIrregularMeasures); });

    registerAction("concert-pitch", &Controller::toggleConcertPitch);

    // KitNote: the swing of the whole score (the setting of Style > Score), one click away.
    registerAction("staff-size-small", [this]() { setStaffSize(1.5); });
    registerAction("staff-size-normal", [this]() { setStaffSize(1.75); });
    registerAction("staff-size-large", [this]() { setStaffSize(2.1); });
    registerAction("swing-off", [this]() { setSwing(kn::engraving::DurationType::V_ZERO); });
    registerAction("swing-eighth", [this]() { setSwing(kn::engraving::DurationType::V_EIGHTH); });
    registerAction("swing-sixteenth", [this]() { setSwing(kn::engraving::DurationType::V_16TH); });

    registerAction("explode", &Interaction::explodeSelectedStaff);
    registerAction("implode", &Interaction::implodeSelectedStaff);
    registerAction("extend-to-next-note", &Interaction::extendToNextNote);
    registerAction("time-delete", &Interaction::removeSelectedRange);
    registerAction("del-empty-measures", &Interaction::removeEmptyTrailingMeasures);
    registerAction("slash-fill", &Interaction::fillSelectionWithSlashes);
    registerAction("slash-rhythm", &Interaction::replaceSelectedNotesWithSlashes);

    // KitNote: measures that repeat the one before them become repeat signs.
    registerAction("measure-repeat-identical", &Interaction::replaceRepeatedMeasuresWithRepeatSigns, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);

    registerAction("measure-repeat-write-out", &Interaction::writeOutMeasureRepeatSigns, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);

    // KitNote: a legend of the drums used, before the first measure.
    registerAction("drum-key-insert", &Interaction::insertDrumKey, PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("drum-key-remove", &Interaction::removeDrumKey, PlayMode::NoPlay, &Controller::isNotNoteInputMode);

    // KitNote: R L R L under the selected notes.
    registerAction("sticking-alternate-right", &Interaction::addAlternatingSticking, true, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("sticking-alternate-left", &Interaction::addAlternatingSticking, false, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);

    // KitNote: changes to the selected drum notes.
    registerAction("drum-change-hihat-to-ride", &Interaction::transformDrumSelection, DrumTransform::HiHatToRide, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-change-ride-to-hihat", &Interaction::transformDrumSelection, DrumTransform::RideToHiHat, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-change-swap-sticking", &Interaction::transformDrumSelection, DrumTransform::SwapSticking, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-change-accent-beats", &Interaction::transformDrumSelection, DrumTransform::AccentBeats, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-change-accent-offbeats", &Interaction::transformDrumSelection, DrumTransform::AccentOffBeats, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-change-accent-clear", &Interaction::transformDrumSelection, DrumTransform::ClearAccents, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);

    // KitNote: 1 e & a under the selected notes.
    registerAction("counting-add", &Interaction::addBeatCounting, PlayMode::NoPlay, &Controller::isNotNoteInputMode);

    // KitNote: grooves and fills for the selected measures.
    registerAction("drum-pattern-rock-8", &Interaction::fillSelectionWithDrumPattern, DrumPattern::Rock8, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-rock-8-push", &Interaction::fillSelectionWithDrumPattern, DrumPattern::Rock8Push, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-rock-16", &Interaction::fillSelectionWithDrumPattern, DrumPattern::Rock16, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-quarter", &Interaction::fillSelectionWithDrumPattern, DrumPattern::Quarter, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-four-floor", &Interaction::fillSelectionWithDrumPattern, DrumPattern::FourOnFloor, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-half-time", &Interaction::fillSelectionWithDrumPattern, DrumPattern::HalfTime, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-ride-8", &Interaction::fillSelectionWithDrumPattern, DrumPattern::Ride8, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-crash-rock-8", &Interaction::fillSelectionWithDrumPattern, DrumPattern::CrashRock8, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-snare-fill", &Interaction::fillSelectionWithDrumPattern, DrumPattern::SnareFill, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    // KitNote: snare rudiments with their sticking.
    registerAction("drum-rudiment-single-stroke", &Interaction::fillSelectionWithDrumPattern, DrumPattern::RudimentSingleStroke,
                   PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("drum-rudiment-double-stroke", &Interaction::fillSelectionWithDrumPattern, DrumPattern::RudimentDoubleStroke,
                   PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("drum-rudiment-paradiddle", &Interaction::fillSelectionWithDrumPattern, DrumPattern::RudimentParadiddle,
                   PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("drum-rudiment-stroke-triplets", &Interaction::fillSelectionWithDrumPattern, DrumPattern::RudimentStrokeTriplets,
                   PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("drum-rudiment-double-paradiddle", &Interaction::fillSelectionWithDrumPattern, DrumPattern::RudimentDoubleParadiddle,
                   PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("drum-rudiment-paradiddle-diddle", &Interaction::fillSelectionWithDrumPattern, DrumPattern::RudimentParadiddleDiddle,
                   PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("drum-rudiment-flams", &Interaction::fillSelectionWithDrumPattern, DrumPattern::RudimentFlams,
                   PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("drum-rudiment-flam-tap", &Interaction::fillSelectionWithDrumPattern, DrumPattern::RudimentFlamTap,
                   PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("drum-rudiment-flam-accent", &Interaction::fillSelectionWithDrumPattern, DrumPattern::RudimentFlamAccent,
                   PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-tom-fill", &Interaction::fillSelectionWithDrumPattern, DrumPattern::TomFill, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-half-bar-fill", &Interaction::fillSelectionWithDrumPattern, DrumPattern::HalfBarFill, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-shuffle", &Interaction::fillSelectionWithDrumPattern, DrumPattern::Shuffle, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-triplet-feel", &Interaction::fillSelectionWithDrumPattern, DrumPattern::TripletFeel, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-triplet-fill", &Interaction::fillSelectionWithDrumPattern, DrumPattern::TripletFill, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-disco", &Interaction::fillSelectionWithDrumPattern, DrumPattern::Disco, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-bossa-nova", &Interaction::fillSelectionWithDrumPattern, DrumPattern::BossaNova, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-six-eight", &Interaction::fillSelectionWithDrumPattern, DrumPattern::SixEight, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-motown", &Interaction::fillSelectionWithDrumPattern, DrumPattern::Motown, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-jazz-ride", &Interaction::fillSelectionWithDrumPattern, DrumPattern::JazzRide, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("drum-pattern-reggae", &Interaction::fillSelectionWithDrumPattern, DrumPattern::ReggaeOneDrop, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("pitch-spell", &Interaction::spellPitches);
    registerAction("pitch-spell-sharps", &Interaction::spellPitchesWithSharps);
    registerAction("pitch-spell-flats", &Interaction::spellPitchesWithFlats);
    registerAction("reset-groupings", &Interaction::regroupNotesAndRests);
    registerAction("resequence-rehearsal-marks", &Interaction::resequenceRehearsalMarks);

    registerAction("unroll-repeats", &Controller::unrollRepeats);

    registerAction("copy-lyrics-to-clipboard", &Interaction::copyLyrics);
    registerAction("drum-tab-copy", &Interaction::copyDrumTab);     // KitNote
    registerAction("drum-tab-paste", &Interaction::pasteDrumTab, PlayMode::NoPlay, &Controller::isNotNoteInputMode);
    registerAction("acciaccatura", &Interaction::addGraceNotesToSelectedNotes, GraceNoteType::ACCIACCATURA);
    registerAction("appoggiatura", &Interaction::addGraceNotesToSelectedNotes, GraceNoteType::APPOGGIATURA);
    registerAction("grace4", &Interaction::addGraceNotesToSelectedNotes, GraceNoteType::GRACE4);
    registerAction("grace16", &Interaction::addGraceNotesToSelectedNotes, GraceNoteType::GRACE16);
    registerAction("grace32", &Interaction::addGraceNotesToSelectedNotes, GraceNoteType::GRACE32);
    registerAction("grace8after", &Interaction::addGraceNotesToSelectedNotes, GraceNoteType::GRACE8_AFTER);
    registerAction("grace16after", &Interaction::addGraceNotesToSelectedNotes, GraceNoteType::GRACE16_AFTER);
    registerAction("grace32after", &Interaction::addGraceNotesToSelectedNotes, GraceNoteType::GRACE32_AFTER);

    registerAction("beam-auto", &Interaction::addBeamToSelectedChordRests, BeamMode::AUTO);
    registerAction("beam-none", &Interaction::addBeamToSelectedChordRests, BeamMode::NONE);
    registerAction("beam-break-left", &Interaction::addBeamToSelectedChordRests, BeamMode::BEGIN);
    registerAction("beam-break-inner-8th", &Interaction::addBeamToSelectedChordRests, BeamMode::BEGIN16);
    registerAction("beam-break-inner-16th", &Interaction::addBeamToSelectedChordRests, BeamMode::BEGIN32);
    registerAction("beam-join", &Interaction::addBeamToSelectedChordRests, BeamMode::MID);

    registerAction("add-brackets", &Interaction::addBracketsToSelection, BracketsType::Brackets);
    registerAction("add-parentheses", &Interaction::addBracketsToSelection, BracketsType::Parentheses);
    registerAction("add-braces", &Interaction::addBracketsToSelection, BracketsType::Braces);

    registerAction("enh-both", &Interaction::changeEnharmonicSpelling, true);
    registerAction("enh-current", &Interaction::changeEnharmonicSpelling, false);

    registerAction("edit-element", &Controller::startEditSelectedElement);
    registerAction("edit-text", &Controller::startEditSelectedText);

    registerAction("text-b", &Interaction::toggleBold, &Controller::isEditingText);
    registerAction("text-i", &Interaction::toggleItalic, &Controller::isEditingText);
    registerAction("text-u", &Interaction::toggleUnderline, &Controller::isEditingText);
    registerAction("text-s", &Interaction::toggleStrike, &Controller::isEditingText);
    registerAction("text-sub", &Interaction::toggleSubScript, &Controller::isEditingText);
    registerAction("text-sup", &Interaction::toggleSuperScript, &Controller::isEditingText);

    registerAction("select-next-measure", &Interaction::addToSelection, MoveDirection::Right, MoveSelectionType::Measure, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("select-prev-measure", &Interaction::addToSelection, MoveDirection::Left, MoveSelectionType::Measure, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("select-begin-line", &Interaction::expandSelection, ExpandSelectionMode::BeginSystem, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("select-end-line", &Interaction::expandSelection, ExpandSelectionMode::EndSystem, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("select-begin-score", &Interaction::expandSelection, ExpandSelectionMode::BeginScore, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("select-end-score", &Interaction::expandSelection, ExpandSelectionMode::EndScore, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("select-staff-above", &Interaction::addToSelection, MoveDirection::Up, MoveSelectionType::Track, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("select-staff-below", &Interaction::addToSelection, MoveDirection::Down, MoveSelectionType::Track, PlayMode::NoPlay,
                   &Controller::isNotNoteInputMode);
    registerAction("top-staff", &Interaction::selectTopStaff, PlayMode::PlayChord);
    registerAction("empty-trailing-measure", &Interaction::selectEmptyTrailingMeasure);
    registerAction("pitch-up-diatonic", &Controller::movePitchDiatonic, MoveDirection::Up, false);
    registerAction("pitch-down-diatonic", &Controller::movePitchDiatonic, MoveDirection::Down, false);

    registerAction("repeat-sel", &Controller::repeatSelection);

    registerAction("add-turn", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentTurn);
    registerAction("add-turn-inverted", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentTurnInverted);
    registerAction("add-turn-slash", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentTurnSlash);
    registerAction("add-turn-up", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentTurnUp);
    registerAction("add-turn-inverted-up", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentTurnUpS);
    registerAction("add-trill", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentTrill);
    registerAction("add-short-trill", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentShortTrill);
    registerAction("add-mordent", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentMordent);
    registerAction("add-haydn", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentHaydn);
    registerAction("add-tremblement", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentTremblement);
    registerAction("add-prall-mordent", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentPrallMordent);
    registerAction("add-shake", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentShake3);
    registerAction("add-shake-muffat", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentShakeMuffat1);
    registerAction("add-tremblement-couperin", &Interaction::toggleOrnament, kn::engraving::SymId::ornamentTremblementCouperin);

    registerAction("add-up-bow", &Interaction::toggleArticulation, kn::engraving::SymId::stringsUpBow);
    registerAction("add-down-bow", &Interaction::toggleArticulation, kn::engraving::SymId::stringsDownBow);
    registerAction("transpose-up", &Interaction::transposeSemitone, 1, PlayMode::PlayNote);
    registerAction("transpose-down", &Interaction::transposeSemitone, -1, PlayMode::PlayNote);
    registerAction("toggle-insert-mode", [this]() { toggleNoteInputInsert(); }, &NotationActionController::isNotEditingElement);

    registerAction("get-location", &Interaction::getLocation, &Controller::isNotationPage);
    registerAction("toggle-mmrest", &Interaction::execute, &kn::engraving::Score::cmdToggleMmrest,
                   TranslatableString("undoableAction", "Toggle multimeasure rests"));
    registerAction("toggle-hide-empty", &Interaction::execute, &kn::engraving::Score::cmdToggleHideEmpty,
                   TranslatableString("undoableAction", "Toggle empty staves"));

    registerAction("mirror-note", &Interaction::execute, &kn::engraving::Score::cmdMirrorNoteHead,
                   TranslatableString("undoableAction", "Mirror notehead"));

    registerAction("clef-violin", [this]() { insertClef(kn::engraving::ClefType::G); });
    registerAction("clef-bass", [this]() { insertClef(kn::engraving::ClefType::F); });

    registerAction("sharp2-post", &Interaction::changeAccidental, kn::engraving::AccidentalType::SHARP2, PlayMode::PlayNote);
    registerAction("sharp-post", &Interaction::changeAccidental, kn::engraving::AccidentalType::SHARP, PlayMode::PlayNote);
    registerAction("nat-post", &Interaction::changeAccidental, kn::engraving::AccidentalType::NATURAL, PlayMode::PlayNote);
    registerAction("flat-post", &Interaction::changeAccidental, kn::engraving::AccidentalType::FLAT, PlayMode::PlayNote);
    registerAction("flat2-post", &Interaction::changeAccidental, kn::engraving::AccidentalType::FLAT2, PlayMode::PlayNote);
    registerAction("pitch-up-diatonic-alterations", &Interaction::transposeDiatonicAlterations, kn::engraving::TransposeDirection::UP,
                   PlayMode::PlayNote);
    registerAction("pitch-down-diatonic-alterations", &Interaction::transposeDiatonicAlterations, kn::engraving::TransposeDirection::DOWN,
                   PlayMode::PlayNote);
    registerAction("full-measure-rest", &Interaction::execute, &kn::engraving::Score::cmdFullMeasureRest,
                   TranslatableString("undoableAction", "Enter full-measure rest"));
    registerAction("set-visible", &Interaction::execute, &kn::engraving::Score::cmdSetVisible,
                   TranslatableString("undoableAction", "Make element(s) visible"));
    registerAction("unset-visible", &Interaction::execute, &kn::engraving::Score::cmdUnsetVisible,
                   TranslatableString("undoableAction", "Make element(s) invisible"));
    registerAction("toggle-autoplace", &Interaction::toggleAutoplace, false);
    registerAction("autoplace-enabled", &Interaction::toggleAutoplace, true);

    for (int i = MIN_NOTES_INTERVAL; i <= MAX_NOTES_INTERVAL; ++i) {
        if (isNotesIntervalValid(i)) {
            registerAction("interval" + std::to_string(i), &Interaction::addIntervalToSelectedNotes, i, PlayMode::PlayChord);
        }
    }

    for (voice_idx_t i = 0; i < kn::engraving::VOICES; ++i) {
        registerAction("voice-" + std::to_string(i + 1), [this, i]() { changeVoice(static_cast<int>(i)); });
    }

    registerAction("voice-assignment-all-in-instrument", &Interaction::changeSelectedElementsVoiceAssignment,
                   VoiceAssignment::ALL_VOICE_IN_INSTRUMENT);
    registerAction("voice-assignment-all-in-staff", &Interaction::changeSelectedElementsVoiceAssignment,
                   VoiceAssignment::ALL_VOICE_IN_STAFF);

    // TAB
    registerAction("string-above", &Controller::move, MoveDirection::Up, false, &Controller::isTablatureStaff);
    registerAction("string-below", &Controller::move, MoveDirection::Down, false, &Controller::isTablatureStaff);
    registerTabPadNoteAction("pad-note-1-TAB", Pad::NOTE1);
    registerTabPadNoteAction("pad-note-2-TAB", Pad::NOTE2);
    registerTabPadNoteAction("pad-note-4-TAB", Pad::NOTE4);
    registerTabPadNoteAction("pad-note-8-TAB", Pad::NOTE8);
    registerTabPadNoteAction("pad-note-16-TAB", Pad::NOTE16);
    registerTabPadNoteAction("pad-note-32-TAB", Pad::NOTE32);
    registerTabPadNoteAction("pad-note-64-TAB", Pad::NOTE64);
    registerTabPadNoteAction("pad-note-128-TAB", Pad::NOTE128);
    registerTabPadNoteAction("pad-note-256-TAB", Pad::NOTE256);
    registerTabPadNoteAction("pad-note-512-TAB", Pad::NOTE512);
    registerTabPadNoteAction("pad-note-1024-TAB", Pad::NOTE1024);
    registerAction("rest-TAB", &Interaction::putRestToSelection);

    registerAction("standard-bend", [this]() { addGuitarBend(GuitarBendType::BEND); });
    registerAction("pre-bend",  [this]() { addGuitarBend(GuitarBendType::PRE_BEND); });
    registerAction("grace-note-bend",  [this]() { addGuitarBend(GuitarBendType::GRACE_NOTE_BEND); });
    registerAction("slight-bend",  [this]() { addGuitarBend(GuitarBendType::SLIGHT_BEND); });

    registerAction("dive", [this]() { addGuitarBend(GuitarBendType::DIVE); });
    registerAction("pre-dive",  [this]() { addGuitarBend(GuitarBendType::PRE_DIVE); });
    registerAction("dip",  [this]() { addGuitarBend(GuitarBendType::DIP); });
    registerAction("scoop",  [this]() { addGuitarBend(GuitarBendType::SCOOP); });

    for (int i = 0; i < MAX_FRET; ++i) {
        registerAction("fret-" + std::to_string(i), [i, this]() { addFret(i); }, &Controller::isTablatureStaff);
    }

    // listen on state changes
    globalContext()->currentNotationChanged().onNotify(this, [this]() {
        auto notation = globalContext()->currentNotation();
        if (notation) {
            notation->interaction()->noteInput()->stateChanged().onNotify(this, [this]() {
                m_currentNotationNoteInputChanged.notify();
            }, Asyncable::Mode::SetReplace);
        }
        m_currentNotationNoteInputChanged.notify();
    });

    // Register engraving debugging options actions
    for (auto& [code, member] : engravingDebuggingActions) {
        dispatcher()->reg(this, code, [this, member = member]() {
            EngravingDebuggingOptions options = engravingConfiguration()->debuggingOptions();
            bool showGapRests = options.showGapRests;
            options.*member = !(options.*member);
            engravingConfiguration()->setDebuggingOptions(options);
            if (options.showGapRests != showGapRests) {
                currentNotation()->interaction()->toggleDebugShowGapRests();
            }
        });
    }
    dispatcher()->reg(this, "check-for-score-corruptions", [this] { checkForScoreCorruptions(); });
}

bool NotationActionController::canReceiveAction(const ActionCode& code) const
{
    // If no notation is loaded, we cannot handle any action.
    auto masterNotation = currentMasterNotation();
    if (!masterNotation) {
        return false;
    }

    if (code == UNDO_ACTION_CODE) {
        return canUndo();
    }

    if (code == REDO_ACTION_CODE) {
        return canRedo();
    }

    // Actions other than undo and redo can only be handled when the current
    // notation contains at least one part.
    if (!masterNotation->hasParts()) {
        return false;
    }

    auto iter = m_isEnabledMap.find(code);
    if (iter != m_isEnabledMap.end()) {
        bool enabled = iter->second();
        return enabled;
    }

    return true;
}

INotationPtr NotationActionController::currentNotation() const
{
    return globalContext()->currentNotation();
}

IMasterNotationPtr NotationActionController::currentMasterNotation() const
{
    return globalContext()->currentMasterNotation();
}

INotationInteractionPtr NotationActionController::currentNotationInteraction() const
{
    return currentNotation() ? currentNotation()->interaction() : nullptr;
}

INotationSelectionPtr NotationActionController::currentNotationSelection() const
{
    return currentNotationInteraction() ? currentNotationInteraction()->selection() : nullptr;
}

INotationElementsPtr NotationActionController::currentNotationElements() const
{
    return currentNotation() ? currentNotation()->elements() : nullptr;
}

kfw::async::Notification NotationActionController::currentNotationChanged() const
{
    return globalContext()->currentNotationChanged();
}

INotationNoteInputPtr NotationActionController::currentNotationNoteInput() const
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return nullptr;
    }

    return interaction->noteInput();
}

kfw::async::Notification NotationActionController::currentNotationNoteInputChanged() const
{
    return m_currentNotationNoteInputChanged;
}

INotationUndoStackPtr NotationActionController::currentNotationUndoStack() const
{
    auto notation = currentNotation();
    if (!notation) {
        return nullptr;
    }

    return notation->undoStack();
}

INotationMidiInputPtr NotationActionController::currentNotationMidiInput() const
{
    auto notation = currentNotation();
    if (!notation) {
        return nullptr;
    }

    return notation->midiInput();
}

kn::engraving::Score* NotationActionController::currentNotationScore() const
{
    return currentNotationElements() ? currentNotationElements()->msScore() : nullptr;
}

INotationStylePtr NotationActionController::currentNotationStyle() const
{
    auto notation = currentNotation();
    if (!notation) {
        return nullptr;
    }

    return notation->style();
}

kfw::async::Notification NotationActionController::currentNotationStyleChanged() const
{
    return currentNotationStyle() ? currentNotationStyle()->styleChanged() : kfw::async::Notification();
}

void NotationActionController::resetState()
{
    TRACEFUNC;

    if (playbackController()->isPlaying()) {
        playbackController()->reset();
    }

    auto noteInput = currentNotationNoteInput();
    if (!noteInput) {
        return;
    }

    if (noteInput->isNoteInputMode()) {
        toggleNoteInput();
        return;
    }

    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    if (interaction->isGripEditStarted()) {
        // Exit grip edit but leave element selected
        interaction->endEditElement();
        return;
    }

    if (interaction->isTextEditingStarted()) {
        interaction->endEditElement();
        return;
    } else if (interaction->isEditingElement()) {
        interaction->endEditElement();
    }

    if (!interaction->selection()->isNone()) {
        interaction->clearSelection();
    }
}

void NotationActionController::toggleNoteInput()
{
    TRACEFUNC;

    INotationNoteInputPtr noteInput = currentNotationNoteInput();
    if (!noteInput) {
        return;
    }

    if (noteInput->isNoteInputMode()) {
        noteInput->endNoteInput();
        return;
    }

    // If the Braille panel or Note Input toolbar has focus, stay there.
    noteInput->startNoteInput(configuration()->defaultNoteInputMethod(), /*focusNotation*/ false);
}

void NotationActionController::toggleNoteInputMethod(NoteInputMethod method)
{
    TRACEFUNC;

    INotationNoteInputPtr noteInput = currentNotationNoteInput();
    if (!noteInput) {
        return;
    }

    if (!noteInput->isNoteInputMode()) {
        noteInput->startNoteInput(method);
    } else if (noteInput->usingNoteInputMethod(method)) {
        toggleNoteInput();
    } else {
        noteInput->setNoteInputMethod(method);
    }
}

void NotationActionController::toggleNoteInputInsert()
{
    if (!currentNotationNoteInput()->usingNoteInputMethod(NoteInputMethod::TIMEWISE)) {
        toggleNoteInputMethod(NoteInputMethod::TIMEWISE);
    } else {
        toggleNoteInputMethod(NoteInputMethod::BY_NOTE_NAME);
    }
}

void NotationActionController::handleNoteAction(NoteName note, NoteAddingMode addingMode)
{
    startNoteInput();

    NoteInputParams params;
    const bool addFlag = addingMode == NoteAddingMode::CurrentChord;
    bool ok = currentNotationScore()->resolveNoteInputParams(static_cast<int>(note), addFlag, params);
    if (!ok) {
        LOGE() << "Could not resolve note input params, note: " << (int)note << ", addFlag: " << addFlag;
        return;
    }

    handleNoteAction(ActionData::make_arg2<NoteInputParams, NoteAddingMode>(params, addingMode));
}

void NotationActionController::handleNoteAction(const kfw::actions::ActionData& args)
{
    TRACEFUNC;

    IF_ASSERT_FAILED(args.count() > 1) {
        return;
    }

    INotationNoteInputPtr noteInput = currentNotationNoteInput();
    if (!noteInput) {
        return;
    }

    startNoteInput();

    const NoteInputParams params = args.arg<NoteInputParams>(0);
    const NoteAddingMode addingMode = args.arg<NoteAddingMode>(1);

    if (addingMode == NoteAddingMode::NextChord) {
        if (noteInput->usingNoteInputMethod(NoteInputMethod::BY_DURATION)) {
            noteInput->setRestMode(false);
            noteInput->setInputNote(params);

            if (configuration()->isPlayPreviewNotesInInputByDuration()) {
                const NoteInputState& state = noteInput->state();
                playbackController()->playNotes(state.notes(), state.staffIdx(), state.segment());
            }
            return;
        }
    }

    noteInput->addNote(params, addingMode);

    seekAndPlaySelectedElement();
}

void NotationActionController::padNote(const Pad& pad)
{
    TRACEFUNC;

    INotationInteractionPtr interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    INotationNoteInputPtr noteInput = interaction->noteInput();
    if (!noteInput) {
        return;
    }

    if (interaction->selection()->isNone()) {
        if (!noteInput->isNoteInputMode() && !startNoteInputAllowed()) {
            return;
        }

        startNoteInput();
    }

    if (pad >= Pad::DOT && pad <= Pad::DOT4) {
        if (!noteInput->isNoteInputMode() || !configuration()->addAccidentalDotsArticulationsToNextNoteEntered()) {
            interaction->toggleDotsForSelection(pad);
            return;
        }
    }

    noteInput->padNote(pad);

    if (noteInput->usingNoteInputMethod(NoteInputMethod::BY_DURATION)
        || noteInput->usingNoteInputMethod(NoteInputMethod::RHYTHM)) {
        seekAndPlaySelectedElement();
    }
}

void NotationActionController::putNote(const ActionData& args)
{
    TRACEFUNC;

    INotationNoteInputPtr noteInput = currentNotationNoteInput();
    if (!noteInput) {
        return;
    }

    IF_ASSERT_FAILED(args.count() > 2) {
        return;
    }

    PointF pos = args.arg<PointF>(0);
    bool replace = args.arg<bool>(1);
    bool insert = args.arg<bool>(2);

    Ret ret = noteInput->putNote(pos, replace, insert);
    if (ret) {
        seekAndPlaySelectedElement();
    }
}

void NotationActionController::removeNote(const ActionData& args)
{
    TRACEFUNC;

    INotationNoteInputPtr noteInput = currentNotationNoteInput();
    if (!noteInput) {
        return;
    }

    IF_ASSERT_FAILED(args.count() == 1) {
        return;
    }

    PointF pos = args.arg<PointF>(0);
    noteInput->removeNote(pos);
    seekSelectedElement();
}

void NotationActionController::toggleAccidental(AccidentalType type)
{
    TRACEFUNC;

    INotationInteractionPtr interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    INotationNoteInputPtr noteInput = interaction->noteInput();
    if (!noteInput) {
        return;
    }

    if (interaction->selection()->isNone()) {
        if (!noteInput->isNoteInputMode() && !startNoteInputAllowed()) {
            return;
        }

        startNoteInput();
    }

    if (noteInput->isNoteInputMode() && configuration()->addAccidentalDotsArticulationsToNextNoteEntered()) {
        noteInput->setAccidental(type);
    } else {
        interaction->toggleAccidentalForSelection(type);
        seekAndPlaySelectedElement();
    }
}

void NotationActionController::toggleArticulation(SymbolId articulationSymbolId)
{
    TRACEFUNC;

    INotationInteractionPtr interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    INotationNoteInputPtr noteInput = interaction->noteInput();
    if (!noteInput) {
        return;
    }

    if (interaction->selection()->isNone()) {
        if (!noteInput->isNoteInputMode() && !startNoteInputAllowed()) {
            return;
        }

        startNoteInput();
    }

    if (noteInput->isNoteInputMode() && configuration()->addAccidentalDotsArticulationsToNextNoteEntered()) {
        noteInput->setArticulation(articulationSymbolId);
    } else {
        interaction->toggleArticulationForSelection(articulationSymbolId);
    }
}

void NotationActionController::putTuplet(const ActionData& data)
{
    IF_ASSERT_FAILED(data.count() == 1) {
        return;
    }

    TupletOptions options = data.arg<TupletOptions>(0);

    putTuplet(options);
}

void NotationActionController::putTuplet(const TupletOptions& options)
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto noteInput = interaction->noteInput();
    if (!noteInput) {
        return;
    }

    if (!interaction->canAddTupletToSelectedChordRests()) {
        interactive()->error(kfw::trc("notation", "Cannot create tuplet"), kfw::trc("notation", "Note value is too short"));
        return;
    }

    if (noteInput->isNoteInputMode()) {
        noteInput->addTuplet(options);
    } else {
        interaction->addTupletToSelectedChordRests(options);
    }
}

void NotationActionController::putTuplet(int tupletCount)
{
    TRACEFUNC;

    TupletOptions options;
    options.ratio.setNumerator(tupletCount);
    options.ratio.setDenominator(2);
    options.autoBaseLen = true;
    // get the bracket type from score style settings
    if (INotationStylePtr style = currentNotationStyle()) {
        int bracketType = style->styleValue(StyleId::tupletBracketType).toInt();
        options.bracketType = static_cast<TupletBracketType>(bracketType);
        int numberType = style->styleValue(StyleId::tupletNumberType).toInt();
        options.numberType = static_cast<TupletNumberType>(numberType);
    }

    putTuplet(options);
}

void NotationActionController::doubleNoteInputDuration()
{
    TRACEFUNC;

    INotationInteractionPtr interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    INotationNoteInputPtr noteInput = interaction->noteInput();

    if (noteInput->isNoteInputMode()) {
        noteInput->doubleNoteInputDuration();
    } else {
        interaction->increaseDecreaseDuration(-1, false);
    }
}

void NotationActionController::halveNoteInputDuration()
{
    TRACEFUNC;

    INotationInteractionPtr interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    INotationNoteInputPtr noteInput = interaction->noteInput();

    if (noteInput->isNoteInputMode()) {
        noteInput->halveNoteInputDuration();
    } else {
        interaction->increaseDecreaseDuration(1, false);
    }
}

void NotationActionController::realtimeAdvance()
{
    TRACEFUNC;

    INotationMidiInputPtr midiInput = currentNotationMidiInput();
    if (!midiInput) {
        return;
    }

    midiInput->onRealtimeAdvance();
}

bool NotationActionController::moveSelectionAvailable(MoveSelectionType type) const
{
    auto interaction = currentNotationInteraction();
    return interaction && interaction->moveSelectionAvailable(type);
}

void NotationActionController::moveSelection(MoveSelectionType type, MoveDirection direction)
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    interaction->moveSelection(direction, type);
    seekSelectedElement();
}

void NotationActionController::move(MoveDirection direction, bool quickly)
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    const bool previousSelectionExists = currentNotationScore() && currentNotationScore()->selection().currentCR();
    if (interaction->selection()->isNone() && previousSelectionExists) {
        // Try to restore the previous selection...
        interaction->moveSelection(direction, MoveSelectionType::EngravingItem);
        seekAndPlaySelectedElement(true);
        return;
    }

    const EngravingItem* selectedElement = interaction->selection()->element();
    INotationNoteInputPtr noteInput = interaction->noteInput();
    bool playChord = false;

    switch (direction) {
    case MoveDirection::Up:
    case MoveDirection::Down:
        if (!quickly && selectedElement && selectedElement->isLyrics()) {
            interaction->moveLyrics(direction);
        } else if (selectedElement && (selectedElement->isTextBase() || selectedElement->isArticulationFamily())) {
            interaction->nudge(direction, quickly);
        } else if (selectedElement && selectedElement->hasGrips() && interaction->isGripEditStarted()) {
            interaction->nudgeAnchors(direction);
        } else if (noteInput->isNoteInputMode() && noteInput->usingNoteInputMethod(NoteInputMethod::BY_DURATION)) {
            moveInputNotes(direction == MoveDirection::Up, quickly ? PitchMode::OCTAVE : PitchMode::DIATONIC);
            return;
        } else if (noteInput->isNoteInputMode() && noteInput->state().staffGroup() == kn::engraving::StaffGroup::TAB) {
            if (quickly) {
                interaction->movePitch(direction, PitchMode::OCTAVE);
            }
            interaction->moveSelection(direction, MoveSelectionType::String);
            return;
        } else if (interaction->selection()->isNone()) {
            interaction->selectFirstElement(false);
        } else {
            interaction->movePitch(direction, quickly ? PitchMode::OCTAVE : PitchMode::CHROMATIC);
        }
        break;
    case MoveDirection::Right:
    case MoveDirection::Left:
        if (playbackController()->isPlaying()) {
            MeasureBeat beat = playbackController()->currentBeat();
            int targetBeatIdx = static_cast<int>(beat.beat);
            int targetMeasureIdx = beat.measureIndex;
            int increment = (direction == MoveDirection::Right ? 1 : -1);

            if (quickly) {
                targetBeatIdx = 0;
                targetMeasureIdx += increment;
                if (targetMeasureIdx > beat.maxMeasureIndex) {
                    targetMeasureIdx = beat.maxMeasureIndex;
                } else if (targetMeasureIdx < 0) {
                    targetMeasureIdx = 0;
                }
            } else {
                targetBeatIdx += increment;
                if (targetBeatIdx > beat.maxBeatIndex) {
                    targetBeatIdx = 0;
                    targetMeasureIdx += 1;
                } else if (targetBeatIdx < 0) {
                    targetMeasureIdx -= 1;

                    // Set target beat to max beat of previous bar
                    engraving::TimeSigMap* timeSigMap = currentMasterNotation()->masterScore()->sigmap();
                    int targetBarStartTick = timeSigMap->bar2tick(targetMeasureIdx, 0);
                    targetBeatIdx = timeSigMap->timesig(Fraction::fromTicks(targetBarStartTick)).timesig().numerator() - 1;
                }
            }

            playbackController()->seekBeat(targetMeasureIdx, targetBeatIdx);
            return;
        }

        if (interaction->isTextEditingStarted() && textNavigationAvailable()) {
            navigateToTextElementInNearMeasure(direction);
            return;
        }

        if (selectedElement && selectedElement->isTextBase()) {
            interaction->nudge(direction, quickly);
        } else if (selectedElement && selectedElement->hasGrips() && interaction->isGripEditStarted()) {
            interaction->nudgeAnchors(direction);
        } else {
            if (interaction->selection()->isNone()) {
                interaction->selectFirstElement(false);
            }
            interaction->moveSelection(direction, quickly ? MoveSelectionType::Measure : MoveSelectionType::Chord);
            playChord = true;
        }
        break;
    case MoveDirection::Undefined:
        break;
    }

    seekAndPlaySelectedElement(playChord);
}

void NotationActionController::moveInputNotes(bool up, PitchMode mode)
{
    INotationInteractionPtr interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    interaction->noteInput()->setRestMode(false);
    interaction->noteInput()->moveInputNotes(up, mode);

    if (configuration()->isPlayPreviewNotesInInputByDuration()) {
        const NoteInputState& state = interaction->noteInput()->state();
        playbackController()->playNotes(state.notes(), state.staffIdx(), state.segment());
    }
}

void NotationActionController::movePitchDiatonic(MoveDirection direction, bool)
{
    INotationInteractionPtr interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    INotationNoteInputPtr noteInput = interaction->noteInput();
    if (noteInput->isNoteInputMode() && noteInput->usingNoteInputMethod(NoteInputMethod::BY_DURATION)) {
        moveInputNotes(direction == MoveDirection::Up, PitchMode::DIATONIC);
        return;
    }

    interaction->movePitch(direction, PitchMode::DIATONIC);
    seekAndPlaySelectedElement(true);
}

void NotationActionController::moveWithinChord(MoveDirection direction)
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    interaction->moveChordNoteSelection(direction);
    seekAndPlaySelectedElement(DONT_PLAY_CHORD);
}

void NotationActionController::selectTopOrBottomOfChord(MoveDirection direction)
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    interaction->selectTopOrBottomOfChord(direction);
    seekAndPlaySelectedElement(DONT_PLAY_CHORD);
}

void NotationActionController::changeVoice(voice_idx_t voiceIndex)
{
    TRACEFUNC;

    INotationInteractionPtr interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    INotationNoteInputPtr noteInput = interaction->noteInput();
    if (!noteInput) {
        return;
    }

    if (interaction->selection()->isNone()) {
        if (!noteInput->isNoteInputMode() && !startNoteInputAllowed()) {
            return;
        }

        startNoteInput();
    }

    noteInput->setCurrentVoice(voiceIndex);

    if (!noteInput->isNoteInputMode()) {
        interaction->changeSelectedElementsVoice(voiceIndex);
    }
}

void NotationActionController::cutSelection()
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }
    interaction->copySelection();
    interaction->deleteSelection();
}

void NotationActionController::repeatSelection()
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }
    interaction->repeatSelection();
    seekAndPlaySelectedElement(true);
}

void NotationActionController::pasteSelection(PastingType type)
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    Fraction scale = resolvePastingScale(interaction, type);
    interaction->pasteSelection(scale);

    seekAndPlaySelectedElement(DONT_PLAY_CHORD);
}

Fraction NotationActionController::resolvePastingScale(const INotationInteractionPtr& interaction, PastingType type) const
{
    TRACEFUNC;
    const Fraction DEFAULT_SCALE(1, 1);

    switch (type) {
    case PastingType::Default: return DEFAULT_SCALE;
    case PastingType::Half: return Fraction(1, 2);
    case PastingType::Double: return Fraction(2, 1);
    case PastingType::Special:
        Fraction scale = DEFAULT_SCALE;
        Fraction duration = interaction->noteInput()->state().duration().fraction();

        if (duration.isValid() && !duration.isZero()) {
            scale = duration * 4;
            scale.reduce();
        }

        return scale;
    }

    return DEFAULT_SCALE;
}

void NotationActionController::addTie()
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto noteInput = interaction->noteInput();
    if (!noteInput) {
        return;
    }

    if (noteInput->isNoteInputMode()) {
        noteInput->addTie();
        seekAndPlaySelectedElement(true);
    } else {
        interaction->addTieToSelection();
    }
}

void NotationActionController::chordTie()
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto noteInput = interaction->noteInput();
    if (!noteInput) {
        return;
    }

    if (noteInput->isNoteInputMode()) {
        noteInput->addTie();
        seekAndPlaySelectedElement(true);
    } else {
        interaction->addTiedNoteToChord();
    }
}

void NotationActionController::addLaissezVib()
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto noteInput = interaction->noteInput();
    if (!noteInput) {
        return;
    }

    if (noteInput->isNoteInputMode()) {
        noteInput->addLaissezVib();
        seekAndPlaySelectedElement(true);
    } else {
        interaction->addLaissezVibToSelection();
    }
}

void NotationActionController::addSlur()
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto noteInput = interaction->noteInput();
    if (!noteInput) {
        return;
    }

    if (noteInput->isNoteInputMode() && noteInput->state().slur()) {
        noteInput->resetSlur();
    } else {
        interaction->addSlurToSelection();
    }
}

void NotationActionController::addHammerOnPullOff()
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    interaction->addHammerOnPullOffToSelection();
}

void NotationActionController::addFret(int num)
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    interaction->addFret(num);
    seekAndPlaySelectedElement(currentNotationScore()->playChord());
}

void NotationActionController::insertClef(kn::engraving::ClefType type)
{
    INotationInteractionPtr interaction = currentNotationInteraction();
    if (!interaction || !interaction->canInsertClef(type)) {
        return;
    }

    interaction->insertClef(type);
}

void NotationActionController::addText(TextStyleType type)
{
    TRACEFUNC;

    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    EngravingItem* item = nullptr;

    const INotationSelectionPtr sel = interaction->selection();
    if (sel->isRange()) {
        const INotationSelectionRangePtr range = sel->range();
        item = range->rangeStartSegment()->firstElementForNavigation(range->startStaffIndex());
    } else {
        item = interaction->contextItem();
    }

    if (isVerticalBoxTextStyle(type)) {
        if (!item || !item->isVBox()) {
            interaction->addTextToTopFrame(type);
            return;
        }
    }

    interaction->addTextToItem(type, item);
}

void NotationActionController::addImage()
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    EngravingItem* item = interaction->contextItem();
    if (!interaction->canAddImageToItem(item)) {
        return;
    }

    std::vector<std::string> filter
        = { kfw::trc("notation", "All Supported Files") + " (*.svg *.svgz *.jpg *.jpeg *.png *.bmp *.tif *.tiff)",
            kfw::trc("notation", "Scalable Vector Graphics") + " (*.svg *.svgz)",
            kfw::trc("notation", "JPEG") + " (*.jpg *.jpeg)",
            kfw::trc("notation", "PNG Bitmap Graphic") + " (*.png)",
            kfw::trc("notation", "Bitmap") + " (*.bmp)",
            kfw::trc("notation", "TIFF") + " (*.tif *.tiff)",
            kfw::trc("notation", "All") + " (*)" };

    kfw::io::path_t path = interactive()->selectOpeningFileSync(kfw::trc("notation", "Insert Image"), "", filter);
    interaction->addImageToItem(path, item);
}

void NotationActionController::addFiguredBass()
{
    TRACEFUNC;

    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    interaction->addFiguredBass();
}

void NotationActionController::addGuitarBend(GuitarBendType bendType)
{
    TRACEFUNC;

    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    interaction->addGuitarBend(bendType);
}

void NotationActionController::addFretboardDiagram()
{
    TRACEFUNC;

    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    interaction->addFretboardDiagram();
}

void NotationActionController::selectAllSimilarElements()
{
    TRACEFUNC;
    auto notationElements = currentNotationElements();
    auto interaction = currentNotationInteraction();
    if (!notationElements || !interaction) {
        return;
    }

    EngravingItem* selectedElement = interaction->selection()->element();
    if (!selectedElement) {
        return;
    }

    FilterElementsOptions options = elementsFilterOptions(selectedElement);
    std::vector<EngravingItem*> elements = notationElements->elements(options);
    if (elements.empty()) {
        return;
    }

    interaction->clearSelection();

    interaction->select(elements, SelectType::ADD);
}

void NotationActionController::selectAllSimilarElementsInStaff()
{
    TRACEFUNC;
    auto notationElements = currentNotationElements();
    auto interaction = currentNotationInteraction();
    if (!notationElements || !interaction) {
        return;
    }

    EngravingItem* selectedElement = interaction->selection()->element();
    if (!selectedElement) {
        return;
    }

    FilterElementsOptions options = elementsFilterOptions(selectedElement);
    options.staffStart = static_cast<int>(selectedElement->staffIdx());
    options.staffEnd = options.staffStart + 1;

    std::vector<EngravingItem*> elements = notationElements->elements(options);
    if (elements.empty()) {
        return;
    }

    interaction->clearSelection();

    interaction->select(elements, SelectType::ADD);
}

void NotationActionController::selectAllSimilarElementsInRange()
{
    auto elements = currentNotationElements();
    kn::engraving::EngravingItem* lastHit = currentNotationSelection()->lastElementHit();
    if (!elements || !lastHit) {
        return;
    }

    kn::engraving::Score* score = elements->msScore();
    score->selectSimilarInRange(lastHit);
    if (score->selectionChanged()) {
        currentNotationInteraction()->selectionChanged().notify();
    }
}

void NotationActionController::openSelectionMoreOptions()
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto item = interaction->contextItem();
    if (!item) {
        return;
    }

    bool noteSelected = item->isNote();

    if (noteSelected) {
        interactive()->open("kitnote://notation/selectnote");
    } else {
        interactive()->open("kitnote://notation/selectelement");
    }
}

void NotationActionController::startEditSelectedElement(const ActionData& args)
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto selection = interaction->selection();
    if (!selection) {
        return;
    }

    kn::engraving::EngravingItem* element = selection->element();
    if (!element) {
        return;
    }

    if (element->isInstrumentName()) {
        openStaffProperties();
        return;
    }

    if (elementHasPopup(element) && !interaction->textEditingAllowed(element)) {
        dispatcher()->dispatch("notation-popup-menu", ActionData::make_arg1<EngravingItem*>(element));
        return;
    }

    if (element->isText()) {
        TextStyleType styleType = kn::engraving::toText(element)->textStyleType();

        if (styleType == kn::engraving::TextStyleType::HEADER || styleType == kn::engraving::TextStyleType::FOOTER) {
            openEditStyleDialog(ActionData::make_arg1<QString>("header-and-footer"));
            return;
        }
    }

    if (interaction->textEditingAllowed(element)) {
        PointF cursorPos = !args.empty() ? args.arg<PointF>(0) : PointF();
        interaction->startEditText(element, cursorPos);
    } else {
        interaction->startEditElement(element);
    }
}

void NotationActionController::startEditSelectedText(const ActionData& args)
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto selection = interaction->selection();
    if (!selection) {
        return;
    }

    kn::engraving::EngravingItem* element = selection->element();

    if (interaction->textEditingAllowed(element)) {
        PointF cursorPos = !args.empty() ? args.arg<PointF>(0) : PointF();
        interaction->startEditText(element, cursorPos);
    }
}

void NotationActionController::addMeasures(const ActionData& actionData, AddBoxesTarget target)
{
    TRACEFUNC;

    if (!actionData.empty()) {
        int count = actionData.arg<int>();
        addBoxes(BoxType::Measure, count, target);
    } else {
        interactive()->open("kitnote://notation/selectmeasurescount")
        .onResolve(this, [this, target](const Val& v) {
            int count = v.toInt();
            addBoxes(BoxType::Measure, count, target);
        });
    }
}

void NotationActionController::addBoxes(BoxType boxType, int count, AddBoxesTarget target)
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    interaction->addBoxes(boxType, count, target);
}

void NotationActionController::unrollRepeats()
{
    NOT_IMPLEMENTED;
    // TODO: https://github.com/musescore/MuseScore/issues/9670
}

void NotationActionController::addStretch(qreal value)
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto selection = currentNotationSelection();
    if (!selection) {
        return;
    }

    if (!selection->isRange()) {
        return;
    }

    interaction->addStretch(value);
}

void NotationActionController::resetStretch()
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto selection = currentNotationSelection();
    if (!selection) {
        return;
    }

    if (!selection->isRange()) {
        return;
    }

    interaction->resetStretch();
}

void NotationActionController::resetBeamMode()
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    auto selection = currentNotationSelection();
    if (!selection) {
        return;
    }

    if (selection->isNone() || selection->isRange()) {
        interaction->resetBeamMode();
    }
}

void NotationActionController::openEditStyleDialog(const ActionData& args)
{
    UriQuery uri("kitnote://notation/style");

    if (args.count() > 0) {
        uri.addParam("currentPageCode", Val(args.arg<QString>(0)));
    }

    if (args.count() > 1) {
        uri.addParam("currentSubPageCode", Val(args.arg<QString>(1)));
    }

    interactive()->open(uri);
}

void NotationActionController::openPageSettingsDialog()
{
    interactive()->open("kitnote://notation/pagesettings");
}

void NotationActionController::openStaffProperties()
{
    interactive()->open("kitnote://notation/staffproperties");
}

void NotationActionController::openEditStringsDialog()
{
    interactive()->open("kitnote://notation/editstrings");
}

void NotationActionController::openBreaksDialog()
{
    interactive()->open("kitnote://notation/breaks");
}

void NotationActionController::openTransposeDialog()
{
    interactive()->open("kitnote://notation/transpose").onResolve(this, [this](const Val&) {
        currentNotationInteraction()->checkAndShowError();
    });
}

void NotationActionController::openPartsDialog()
{
    interactive()->open("kitnote://notation/parts");
}

kfw::io::path_t NotationActionController::selectStyleFile(bool forLoad)
{
    kfw::io::path_t dir = configuration()->userStylesPath();
    std::string filterName = forLoad
                             ? kfw::trc("notation", "MuseScore style files")
                             : kfw::trc("notation", "MuseScore style file");
    std::vector<std::string> filter = { filterName + " (*.mss)" };
    return forLoad
           ? interactive()->selectOpeningFileSync(kfw::trc("notation", "Load style"), dir, filter)
           : interactive()->selectSavingFileSync(kfw::trc("notation", "Save style"), dir, filter);
}

void NotationActionController::loadStyle()
{
    TRACEFUNC;
    auto path = selectStyleFile(true);
    if (!path.empty()) {
        File f(path.toQString());
        if (!f.open(IODevice::ReadOnly) || !kn::engraving::MStyle::isValid(&f)) {
            interactive()->error(kfw::trc("notation", "The style file could not be loaded."),
                                 f.errorString());
            return;
        }
        if (!currentNotationStyle()->loadStyle(path.toQString(), false)) {
            auto promise = interactive()->warning(
                kfw::trc("notation",
                          "Since this style file is from a different version of KitNote, your score is not guaranteed to display correctly."),
                kfw::trc("notation", "Click OK to load anyway."), { IInteractive::Button::Ok, IInteractive::Button::Cancel },
                IInteractive::Button::Ok);

            promise.onResolve(this, [this, path](const IInteractive::Result& res) {
                if (res.isButton(IInteractive::Button::Ok)) {
                    currentNotationStyle()->loadStyle(path.toQString(), true);
                }
            });
        }
    }
}

void NotationActionController::saveStyle()
{
    TRACEFUNC;
    auto path = selectStyleFile(false);
    if (!path.empty()) {
        if (!currentNotationStyle()->saveStyle(path)) {
            interactive()->error(kfw::trc("notation", "The style file could not be saved."),
                                 kfw::trc("notation", "An error occurred."));
        }
    }
}

FilterElementsOptions NotationActionController::elementsFilterOptions(const EngravingItem* element) const
{
    TRACEFUNC;
    FilterElementsOptions options;
    options.elementType = element->type();

    if (element->isNote()) {
        const kn::engraving::Note* note = dynamic_cast<const kn::engraving::Note*>(element);
        if (note->chord()->isGrace()) {
            options.subtype = -1;
        } else {
            options.subtype = element->subtype();
        }
    } else if (element->isHairpinSegment() || element->isHarmony()) {
        options.subtype = element->subtype();
        options.bySubtype = true;
    }

    return options;
}

bool NotationActionController::measureNavigationAvailable() const
{
    return isNotEditingOrHasPopup() || textNavigationAvailable();
}

bool NotationActionController::toggleLayoutBreakAvailable() const
{
    INotationInteractionPtr interaction = currentNotationInteraction();
    return interaction && interaction->toggleLayoutBreakAvailable();
}

bool NotationActionController::textNavigationAvailable() const
{
    return resolveTextNavigationAvailable(TextNavigationType::NearNoteOrRest);
}

bool NotationActionController::textNavigationByBeatsAvailable() const
{
    return resolveTextNavigationAvailable(TextNavigationType::NearBeat);
}

bool NotationActionController::textNavigationByFractionAvailable() const
{
    return resolveTextNavigationAvailable(TextNavigationType::Fraction);
}

bool NotationActionController::resolveTextNavigationAvailable(TextNavigationType type) const
{
    if (!isEditingText()) {
        return false;
    }

    static const QList<kn::engraving::ElementType> allowedElementsForTextNavigation {
        kn::engraving::ElementType::LYRICS,
        kn::engraving::ElementType::HARMONY,
        kn::engraving::ElementType::FIGURED_BASS,
        kn::engraving::ElementType::STICKING,
        kn::engraving::ElementType::FINGERING
    };

    static const QList<kn::engraving::ElementType> allowedElementsForBeatsNavigation {
        kn::engraving::ElementType::HARMONY
    };

    static const QList<kn::engraving::ElementType> allowedElementsForFractionNavigation {
        kn::engraving::ElementType::HARMONY,
        kn::engraving::ElementType::FIGURED_BASS
    };

    const kn::engraving::EngravingItem* element = selectedElement();
    kn::engraving::ElementType elementType = element ? element->type() : kn::engraving::ElementType::INVALID;

    switch (type) {
    case TextNavigationType::NearNoteOrRest:
        return allowedElementsForTextNavigation.contains(elementType);
    case TextNavigationType::NearBeat:
        return allowedElementsForBeatsNavigation.contains(elementType);
    case TextNavigationType::Fraction:
        return allowedElementsForFractionNavigation.contains(elementType);
    }

    return false;
}

void NotationActionController::nextTextElement()
{
    navigateToTextElement(MoveDirection::Right, NEAR_NOTE_OR_REST);
}

void NotationActionController::prevTextElement()
{
    navigateToTextElement(MoveDirection::Left, NEAR_NOTE_OR_REST);
}

void NotationActionController::nextWord()
{
    navigateToTextElement(MoveDirection::Right, NEAR_NOTE_OR_REST, false);
}

void NotationActionController::nextBeatTextElement()
{
    navigateToTextElement(MoveDirection::Right);
}

void NotationActionController::prevBeatTextElement()
{
    navigateToTextElement(MoveDirection::Left);
}

void NotationActionController::navigateToTextElement(MoveDirection direction, bool nearNoteOrRest, bool moveOnly)
{
    const kn::engraving::EngravingItem* element = selectedElement();
    if (!element) {
        return;
    }

    if (element->isLyrics()) {
        currentNotationInteraction()->navigateToLyrics(direction, moveOnly);
    } else if (element->isHarmony()) {
        const Harmony* chordSymbol = editedChordSymbol();

        // otherwise, chord symbol will be deleted when navigating away from it
        const bool canPlay = chordSymbol && !chordSymbol->harmonyName().empty();

        currentNotationInteraction()->navigateToNearHarmony(direction, nearNoteOrRest);

        if (canPlay) {
            playbackController()->playElements({ chordSymbol });
        }
    } else if (element->isFiguredBass()) {
        currentNotationInteraction()->navigateToNearFiguredBass(direction);
    } else {
        currentNotationInteraction()->navigateToNearText(direction);
    }
}

void NotationActionController::navigateToTextElementByFraction(const Fraction& fraction)
{
    const kn::engraving::EngravingItem* element = selectedElement();
    if (!element) {
        return;
    }

    if (element->isHarmony()) {
        const Harmony* chordSymbol = editedChordSymbol();

        // otherwise, chord symbol will be deleted when navigating away from it
        const bool canPlay = chordSymbol && !chordSymbol->harmonyName().empty();

        currentNotationInteraction()->navigateToHarmony(fraction);

        if (canPlay) {
            playbackController()->playElements({ chordSymbol });
        }
    } else if (element->isFiguredBass()) {
        currentNotationInteraction()->navigateToFiguredBass(fraction);
    }
}

void NotationActionController::navigateToTextElementInNearMeasure(MoveDirection direction)
{
    const kn::engraving::EngravingItem* element = selectedElement();
    if (!element) {
        return;
    }

    if (element->isHarmony()) {
        const Harmony* chordSymbol = editedChordSymbol();

        // otherwise, chord symbol will be deleted when navigating away from it
        const bool canPlay = chordSymbol && !chordSymbol->harmonyName().empty();

        currentNotationInteraction()->navigateToHarmonyInNearMeasure(direction);

        if (canPlay) {
            playbackController()->playElements({ chordSymbol });
        }
    } else if (element->isFiguredBass()) {
        currentNotationInteraction()->navigateToFiguredBassInNearMeasure(direction);
    }
}

bool NotationActionController::isEditingText() const
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return false;
    }

    return interaction->isTextEditingStarted();
}

bool NotationActionController::isEditingLyrics() const
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return false;
    }

    return interaction->isTextEditingStarted() && interaction->selection()->element()
           && interaction->selection()->element()->isLyrics();
}

bool NotationActionController::isNoteInputMode() const
{
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return false;
    }

    auto noteInput = interaction->noteInput();
    if (!noteInput) {
        return false;
    }

    return noteInput->isNoteInputMode();
}

bool NotationActionController::isNotNoteInputMode() const
{
    return currentNotationInteraction() && !isNoteInputMode();
}

void NotationActionController::openTupletOtherDialog()
{
    interactive()->open("kitnote://notation/othertupletdialog");
}

void NotationActionController::openStaffTextPropertiesDialog()
{
    interactive()->open("kitnote://notation/stafftextproperties");
}

void NotationActionController::openMeasurePropertiesDialog()
{
    if (currentNotationInteraction()->selectedMeasure() != nullptr) {
        interactive()->open("kitnote://notation/measureproperties");
    }
}

void NotationActionController::openEditGridSizeDialog()
{
    interactive()->open("kitnote://notation/editgridsize");
}

void NotationActionController::openRealizeChordSymbolsDialog()
{
    interactive()->open("kitnote://notation/realizechordsymbols");
}

void NotationActionController::toggleScoreConfig(ScoreConfigType configType)
{
    TRACEFUNC;
    auto interaction = currentNotationInteraction();
    if (!interaction) {
        return;
    }

    ScoreConfig config = interaction->scoreConfig();

    switch (configType) {
    case ScoreConfigType::ShowInvisibleElements:
        config.isShowInvisibleElements = !config.isShowInvisibleElements;
        break;
    case ScoreConfigType::ShowUnprintableElements:
        config.isShowUnprintableElements = !config.isShowUnprintableElements;
        break;
    case ScoreConfigType::ShowFrames:
        config.isShowFrames = !config.isShowFrames;
        break;
    case ScoreConfigType::ShowPageMargins:
        config.isShowPageMargins = !config.isShowPageMargins;
        break;
    case ScoreConfigType::ShowSoundFlags:
        config.isShowSoundFlags = !config.isShowSoundFlags;
        break;
    case ScoreConfigType::MarkIrregularMeasures:
        config.isMarkIrregularMeasures = !config.isMarkIrregularMeasures;
        break;
    }

    interaction->setScoreConfig(config);
    interaction->scoreConfigChanged().send(configType);
}

void NotationActionController::toggleConcertPitch()
{
    TRACEFUNC;
    INotationStylePtr style = currentNotationStyle();
    if (!style) {
        return;
    }

    bool toggle = !style->styleValue(StyleId::concertPitch).toBool();

    const TranslatableString actionName = toggle
                                          ? TranslatableString("undoableAction", "Display concert pitch")
                                          : TranslatableString("undoableAction", "Display transposed");

    currentNotationUndoStack()->prepareChanges(actionName);
    style->setStyleValue(StyleId::concertPitch, toggle);
    currentNotationUndoStack()->commitChanges();
}

// KitNote: sets which notes playback swings - none, eighths or sixteenths.
void NotationActionController::setSwing(kn::engraving::DurationType unit)
{
    INotationStylePtr style = currentNotationStyle();
    if (!style) {
        return;
    }

    // what the style says now; anything but eighths and sixteenths (the default is an empty text) is "off"
    const kfw::ByteArray now = style->styleValue(StyleId::swingUnit).value<String>().toAscii();
    kn::engraving::DurationType current = kn::engraving::TConv::fromXml(now.constChar(), kn::engraving::DurationType::V_INVALID);
    if (current != kn::engraving::DurationType::V_EIGHTH && current != kn::engraving::DurationType::V_16TH) {
        current = kn::engraving::DurationType::V_ZERO;
    }
    if (current == unit) {
        return;
    }

    const String value = String::fromAscii(kn::engraving::TConv::toXml(unit).ascii());

    currentNotationUndoStack()->prepareChanges(TranslatableString("undoableAction", "Change swing"));
    style->setStyleValue(StyleId::swingUnit, value);
    currentNotationUndoStack()->commitChanges();
}

// KitNote: the size of the staves (the space between two lines, in millimetres): a chart read from a
// music stand a step away wants it larger than one on a desk. Fewer measures then go to a line.
void NotationActionController::setStaffSize(double millimetres)
{
    INotationStylePtr style = currentNotationStyle();
    if (!style) {
        return;
    }

    currentNotationUndoStack()->prepareChanges(TranslatableString("undoableAction", "Change staff size"));
    style->setStyleValue(StyleId::spatium, millimetres * kn::engraving::DPMM);
    currentNotationUndoStack()->commitChanges();
}

void NotationActionController::seekAndPlaySelectedElement(bool playChord)
{
    seekSelectedElement();
    playSelectedElement(playChord);
}

void NotationActionController::seekSelectedElement()
{
    const IMasterNotationPtr master = currentMasterNotation();
    if (!master || master->playback()->isLoopEnabled()) {
        return;
    }

    const EngravingItem* element = selectedElement();
    if (!element) {
        return;
    }

    playbackController()->seekElement(element);
}

void NotationActionController::playSelectedElement(bool playChord)
{
    TRACEFUNC;

    const EngravingItem* element = selectedElement();
    if (!element) {
        return;
    }

    if (playChord) {
        element = element->elementBase();
    }

    playbackController()->playElements({ element });

    currentNotationScore()->setPlayChord(false);
    currentNotationScore()->setPlayNote(false);
}

bool NotationActionController::startNoteInputAllowed() const
{
    if (isEditingElement() || QGuiApplication::applicationState() != Qt::ApplicationActive) {
        return false;
    }

    const UiContext& ctx = uiContextResolver()->currentUiContext();
    const INavigationControl* ctrl = navigationController()->activeControl();

    return ctx == ui::UiCtxProjectFocused
           || ctx == ui::UiCtxBrailleFocused
           || (ctrl && ctrl->name().startsWith("note-input")); // Toolbar buttons.
}

void NotationActionController::startNoteInput()
{
    INotationNoteInputPtr noteInput = currentNotationNoteInput();
    if (noteInput) {
        noteInput->startNoteInput(configuration()->defaultNoteInputMethod());
    }
}

bool NotationActionController::hasSelection() const
{
    return currentNotationSelection() ? !currentNotationSelection()->isNone() : false;
}

kn::engraving::EngravingItem* NotationActionController::selectedElement() const
{
    auto selection = currentNotationSelection();
    return selection ? selection->element() : nullptr;
}

bool NotationActionController::noteOrRestSelected() const
{
    if (isNoteInputMode()) {
        return true;
    }

    INotationSelectionPtr selection = currentNotationInteraction() ? currentNotationInteraction()->selection() : nullptr;
    return selection && selection->elementsSelected(NOTE_REST_TYPES);
}

const kn::engraving::Harmony* NotationActionController::editedChordSymbol() const
{
    INotationInteractionPtr interaction = currentNotationInteraction();
    if (!interaction) {
        return nullptr;
    }

    const TextBase* text = interaction->editedText();
    if (!text || !text->isHarmony()) {
        return nullptr;
    }

    return toHarmony(text);
}

bool NotationActionController::elementHasPopup(const EngravingItem* e) const
{
    return AbstractElementPopupModel::hasElementEditPopup(e);
}

bool NotationActionController::canUndo() const
{
    return currentNotationUndoStack() ? currentNotationUndoStack()->canUndo() : false;
}

bool NotationActionController::canRedo() const
{
    return currentNotationUndoStack() ? currentNotationUndoStack()->canRedo() : false;
}

bool NotationActionController::isNotationPage() const
{
    return uiContextResolver()->matchWithCurrent(context::UiCtxProjectOpened);
}

bool NotationActionController::isTablatureStaff() const
{
    return isNotEditingElement() && currentNotationScore()->inputState().staffGroup() == kn::engraving::StaffGroup::TAB;
}

bool NotationActionController::isEditingElement() const
{
    auto interaction = currentNotationInteraction();
    if (interaction) {
        return interaction->isEditingElement() || interaction->isDragStarted();
    }
    return false;
}

bool NotationActionController::isNotEditingElement() const
{
    return !isEditingElement();
}

bool NotationActionController::isNotEditingOrHasPopup() const
{
    const EngravingItem* element = selectedElement();

    if (!element) {
        return isNotEditingElement();
    }

    return elementHasPopup(element) || isNotEditingElement();
}

bool NotationActionController::isToggleVisibleAllowed() const
{
    auto interaction = currentNotationInteraction();
    if (interaction) {
        return !(interaction->isTextEditingStarted());
    }
    return false;
}

void NotationActionController::checkForScoreCorruptions()
{
    project::INotationProjectPtr project = globalContext()->currentProject();
    if (!project) {
        return;
    }

    String fileName = io::filename(project->path()).toString();

    Ret ret = project->masterNotation()->masterScore()->sanityCheck();
    if (ret) {
        std::string title = kfw::mtrc("project", "File “%1” seems not corrupted").arg(fileName).toStdString();
        std::string body = kfw::trc("project", "This file does not seem to contain errors.");
        interactive()->info(title, body);
    } else {
        std::string title = kfw::mtrc("project", "File “%1” is corrupted").arg(fileName).toStdString();
        IInteractive::Text text;
        text.text = kfw::trc("project", "This file contains errors that could cause KitNote to malfunction. "
                                         "Please fix those at the earliest, to prevent crashes and further corruptions.");
        text.detailedText = ret.text();

        interactive()->warning(title, text);
    }
}

void NotationActionController::registerAction(const ActionCode& code,
                                              std::function<void()> handler, bool (NotationActionController::* isEnabled)() const)
{
    m_isEnabledMap[code] = std::bind(isEnabled, this);
    dispatcher()->reg(this, code, handler);
}

void NotationActionController::registerAction(const ActionCode& code,
                                              std::function<void(const ActionData&)> handler,
                                              bool (NotationActionController::* isEnabled)() const)
{
    m_isEnabledMap[code] = std::bind(isEnabled, this);
    dispatcher()->reg(this, code, handler);
}

void NotationActionController::registerAction(const ActionCode& code,
                                              void (NotationActionController::* handler)(const ActionData& data),
                                              bool (NotationActionController::* isEnabled)() const)
{
    m_isEnabledMap[code] = std::bind(isEnabled, this);
    dispatcher()->reg(this, code, this, handler);
}

void NotationActionController::registerAction(const ActionCode& code,
                                              void (NotationActionController::* handler)(),
                                              bool (NotationActionController::* isEnabled)() const)
{
    m_isEnabledMap[code] = std::bind(isEnabled, this);
    dispatcher()->reg(this, code, this, handler);
}

void NotationActionController::registerNoteInputAction(const ActionCode& code, NoteInputMethod inputMethod)
{
    registerAction(code, [this, inputMethod]() { toggleNoteInputMethod(inputMethod); }, &Controller::startNoteInputAllowed);
}

bool NotationActionController::noteInputActionAllowed() const
{
    if (!isNoteInputMode() && !startNoteInputAllowed()) {
        return false;
    }

    return !isTablatureStaff();
}

void NotationActionController::registerNoteAction(const ActionCode& code, NoteName noteName, NoteAddingMode addingMode)
{
    registerAction(code, [this, noteName, addingMode]()
    {
        handleNoteAction(noteName, addingMode);
    }, &Controller::noteInputActionAllowed);
}

void NotationActionController::registerPadNoteAction(const ActionCode& code, Pad padding)
{
    registerAction(code, [this, padding, code]()
    {
        padNote(padding);
    });
}

void NotationActionController::registerTabPadNoteAction(const ActionCode& code, Pad padding)
{
    registerAction(code, [this, padding, code]()
    {
        padNote(padding);
    }, &NotationActionController::isTablatureStaff);
}

void NotationActionController::registerMoveSelectionAction(const ActionCode& code, MoveSelectionType type,
                                                           MoveDirection direction, PlayMode playMode)
{
    auto moveSelectionFunc = [this, type, direction, playMode]() {
        moveSelection(type, direction);

        if (playMode != PlayMode::NoPlay) {
            playSelectedElement(playMode == PlayMode::PlayChord);
        }
    };

    auto moveSelectionAvailableFunc = [this, type]() {
        return moveSelectionAvailable(type);
    };

    m_isEnabledMap[code] = moveSelectionAvailableFunc;
    dispatcher()->reg(this, code, moveSelectionFunc);
}

void NotationActionController::registerAction(const ActionCode& code,
                                              void (INotationInteraction::* handler)(), PlayMode playMode,
                                              bool (NotationActionController::* enabler)() const)
{
    registerAction(code, [this, handler, playMode]()
    {
        INotationPtr notation = currentNotation();
        if (notation) {
            (notation->interaction().get()->*handler)();

            seekSelectedElement();

            if (playMode != PlayMode::NoPlay) {
                playSelectedElement(playMode == PlayMode::PlayChord);
            }
        }
    }, enabler);
}

void NotationActionController::registerAction(const ActionCode& code,
                                              void (NotationActionController::* handler)(MoveDirection,
                                                                                         bool), MoveDirection direction, bool quickly,
                                              bool (NotationActionController::* enabler)() const)
{
    registerAction(code, [this, handler, direction, quickly]() { (this->*handler)(direction, quickly); }, enabler);
}

void NotationActionController::registerAction(const kfw::actions::ActionCode& code,
                                              void (NotationActionController::* handler)(), Ret (INotationInteraction::*enabler)() const)
{
    auto _enabler = [this, enabler]() {
        INotationPtr notation = currentNotation();
        if (notation) {
            return ((*notation->interaction()).*enabler)().success();
        }

        return false;
    };

    m_isEnabledMap[code] = _enabler;
    dispatcher()->reg(this, code, this, handler);
}

void NotationActionController::registerAction(const kfw::actions::ActionCode& code, std::function<void()> handler,
                                              Ret (INotationInteraction::*enabler)() const)
{
    auto _enabler = [this, enabler]() {
        INotationPtr notation = currentNotation();
        if (notation) {
            return ((*notation->interaction()).*enabler)().success();
        }

        return false;
    };

    m_isEnabledMap[code] = _enabler;
    dispatcher()->reg(this, code, handler);
}

void NotationActionController::registerAction(const ActionCode& code,
                                              void (INotationInteraction::* handler)(), bool (NotationActionController::* enabler)() const)
{
    registerAction(code, handler, PlayMode::NoPlay, enabler);
}

template<class P1>
void NotationActionController::registerAction(const ActionCode& code, void (INotationInteraction::* handler)(P1),
                                              P1 param1, PlayMode playMode, bool (NotationActionController::* enabler)() const)
{
    registerAction(code, [this, handler, param1, playMode]()
    {
        INotationPtr notation = currentNotation();
        if (notation) {
            (notation->interaction().get()->*handler)(param1);

            seekSelectedElement();

            if (playMode != PlayMode::NoPlay) {
                playSelectedElement(playMode == PlayMode::PlayChord);
            }
        }
    }, enabler);
}

template<class P1>
void NotationActionController::registerAction(const ActionCode& code,
                                              void (INotationInteraction::* handler)(
                                                  P1), P1 param1, bool (NotationActionController::* enabler)() const)
{
    registerAction(code, handler, param1, PlayMode::NoPlay, enabler);
}

template<typename P1, typename P2, typename Q1, typename Q2>
void NotationActionController::registerAction(const ActionCode& code, void (INotationInteraction::* handler)(P1, P2),
                                              Q1 param1, Q2 param2, PlayMode playMode, bool (NotationActionController::* enabler)() const)
{
    registerAction(code, [this, handler, param1, param2, playMode]()
    {
        INotationPtr notation = currentNotation();
        if (notation) {
            (notation->interaction().get()->*handler)(param1, param2);

            seekSelectedElement();

            if (playMode != PlayMode::NoPlay) {
                playSelectedElement(playMode == PlayMode::PlayChord);
            }
        }
    }, enabler);
}
