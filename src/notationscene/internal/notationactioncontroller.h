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
#include "actions/actionable.h"
#include "actions/actiontypes.h"

#include "modularity/ioc.h"
#include "iinteractive.h"
#include "actions/iactionsdispatcher.h"
#include "ui/inavigationcontroller.h"
#include "ui/iuiactionsregister.h"
#include "context/iglobalcontext.h"
#include "context/iuicontextresolver.h"
#include "playback/iplaybackcontroller.h"
#include "engraving/iengravingconfiguration.h"
#include "notation/inotationconfiguration.h"

#include "notation/inotation.h"

namespace kn::notation {
class NotationActionController : public kfw::actions::Actionable, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> configuration;
    kfw::GlobalInject<engraving::IEngravingConfiguration> engravingConfiguration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<kfw::ui::INavigationController> navigationController = { this };
    kfw::ContextInject<kfw::ui::IUiActionsRegister> actionRegister = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<context::IUiContextResolver> uiContextResolver = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<playback::IPlaybackController> playbackController = { this };

public:

    NotationActionController(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void init();

    bool canReceiveAction(const kfw::actions::ActionCode& code) const override;

    kfw::async::Notification currentNotationChanged() const;

    INotationNoteInputPtr currentNotationNoteInput() const;
    kfw::async::Notification currentNotationNoteInputChanged() const;

    INotationInteractionPtr currentNotationInteraction() const;
    INotationUndoStackPtr currentNotationUndoStack() const;

    INotationStylePtr currentNotationStyle() const;
    kfw::async::Notification currentNotationStyleChanged() const;

    using EngravingDebuggingOptions = engraving::IEngravingConfiguration::DebuggingOptions;
    static const std::unordered_map<kfw::actions::ActionCode, bool EngravingDebuggingOptions::*> engravingDebuggingActions;

private:
    INotationPtr currentNotation() const;
    IMasterNotationPtr currentMasterNotation() const;
    INotationElementsPtr currentNotationElements() const;
    INotationSelectionPtr currentNotationSelection() const;
    INotationMidiInputPtr currentNotationMidiInput() const;

    kn::engraving::Score* currentNotationScore() const;

    void toggleNoteInput();
    void toggleNoteInputMethod(NoteInputMethod method);
    void toggleNoteInputInsert();
    void handleNoteAction(NoteName note, NoteAddingMode addingMode);
    void handleNoteAction(const kfw::actions::ActionData& args);
    void padNote(const Pad& pad);
    void putNote(const kfw::actions::ActionData& args);
    void removeNote(const kfw::actions::ActionData& args);
    void doubleNoteInputDuration();
    void halveNoteInputDuration();
    void realtimeAdvance();

    void toggleAccidental(AccidentalType type);
    void toggleArticulation(SymbolId articulationSymbolId);

    void putTuplet(const kfw::actions::ActionData& data);
    void putTuplet(const TupletOptions& options);
    void putTuplet(int tupletCount);

    bool moveSelectionAvailable(MoveSelectionType type) const;
    void moveSelection(MoveSelectionType type, MoveDirection direction);
    void move(MoveDirection direction, bool quickly = false);
    void moveInputNotes(bool up, PitchMode mode);
    void movePitchDiatonic(MoveDirection direction, bool);
    void moveWithinChord(MoveDirection direction);
    void selectTopOrBottomOfChord(MoveDirection direction);

    void changeVoice(voice_idx_t voiceIndex);

    void cutSelection();
    void repeatSelection();
    void addTie();
    void chordTie();
    void addLaissezVib();
    void addSlur();
    void addHammerOnPullOff();
    void addFret(int num);

    void insertClef(kn::engraving::ClefType type);

    void addText(TextStyleType type);
    void addImage();
    void addFiguredBass();
    void addGuitarBend(GuitarBendType bendType);
    void addFretboardDiagram();

    void selectAllSimilarElements();
    void selectAllSimilarElementsInStaff();
    void selectAllSimilarElementsInRange();
    void openSelectionMoreOptions();

    void startEditSelectedElement(const kfw::actions::ActionData& args);
    void startEditSelectedText(const kfw::actions::ActionData& args);

    void addMeasures(const kfw::actions::ActionData& actionData, AddBoxesTarget target);
    void addBoxes(BoxType boxType, int count, AddBoxesTarget target);

    void addStretch(qreal value);

    void unrollRepeats();

    void resetState();
    void resetStretch();
    void resetBeamMode();

    void openEditStyleDialog(const kfw::actions::ActionData& args);
    void openPageSettingsDialog();
    void openStaffProperties();
    void openEditStringsDialog();
    void openBreaksDialog();
    void openTransposeDialog();
    void openPartsDialog();
    void openTupletOtherDialog();
    void openStaffTextPropertiesDialog();
    void openMeasurePropertiesDialog();
    void openEditGridSizeDialog();
    void openRealizeChordSymbolsDialog();
    kfw::io::path_t selectStyleFile(bool forLoad);
    void loadStyle();
    void saveStyle();

    void toggleScoreConfig(ScoreConfigType configType);
    void toggleConcertPitch();
    void setSwing(kn::engraving::DurationType unit); // KitNote
    void setStaffSize(double millimetres); // KitNote

    void seekAndPlaySelectedElement(bool playChord = true);
    void seekSelectedElement();
    void playSelectedElement(bool playChord = true);

    bool isEditingText() const;
    bool isEditingLyrics() const;
    bool isNoteInputMode() const;
    bool isEditingElement() const;
    bool isNotEditingElement() const;
    bool isNotEditingOrHasPopup() const;
    bool isNotNoteInputMode() const;

    bool isToggleVisibleAllowed() const;

    void pasteSelection(PastingType type = PastingType::Default);
    Fraction resolvePastingScale(const INotationInteractionPtr& interaction, PastingType type) const;

    FilterElementsOptions elementsFilterOptions(const EngravingItem* element) const;

    bool measureNavigationAvailable() const;
    bool toggleLayoutBreakAvailable() const;

    enum class TextNavigationType {
        NearNoteOrRest,
        NearBeat,
        Fraction
    };

    bool textNavigationAvailable() const;
    bool textNavigationByBeatsAvailable() const;
    bool textNavigationByFractionAvailable() const;
    bool resolveTextNavigationAvailable(TextNavigationType type = TextNavigationType::NearNoteOrRest) const;

    void nextTextElement();
    void prevTextElement();
    void nextBeatTextElement();
    void prevBeatTextElement();
    void nextWord();
    void navigateToTextElement(MoveDirection direction, bool nearNoteOrRest = false, bool moveOnly = true);
    void navigateToTextElementByFraction(const Fraction& fraction);
    void navigateToTextElementInNearMeasure(MoveDirection direction);

    bool startNoteInputAllowed() const;
    void startNoteInput();

    bool hasSelection() const;
    kn::engraving::EngravingItem* selectedElement() const;
    bool noteOrRestSelected() const;

    const kn::engraving::Harmony* editedChordSymbol() const;

    bool elementHasPopup(const EngravingItem* e) const;

    bool canUndo() const;
    bool canRedo() const;

    bool isNotationPage() const;
    bool isTablatureStaff() const;

    void checkForScoreCorruptions();

    void registerAction(const kfw::actions::ActionCode&, void (NotationActionController::*)(const kfw::actions::ActionData& data),
                        bool (NotationActionController::*)() const = &NotationActionController::isNotationPage);
    void registerAction(const kfw::actions::ActionCode&, void (NotationActionController::*)(),
                        bool (NotationActionController::*)() const = &NotationActionController::isNotationPage);
    void registerAction(const kfw::actions::ActionCode&, std::function<void()>,
                        bool (NotationActionController::*)() const = &NotationActionController::isNotationPage);
    void registerAction(const kfw::actions::ActionCode&, std::function<void(const kfw::actions::ActionData& data)>,
                        bool (NotationActionController::*)() const = &NotationActionController::isNotationPage);
    void registerAction(const kfw::actions::ActionCode&, void (NotationActionController::*)(MoveDirection, bool), MoveDirection, bool,
                        bool (NotationActionController::*)() const = &NotationActionController::isNotEditingElement);
    void registerAction(const kfw::actions::ActionCode&, void (NotationActionController::*)(),
                        kfw::Ret (INotationInteraction::*)() const);
    void registerAction(const kfw::actions::ActionCode&, std::function<void()>,
                        kfw::Ret (INotationInteraction::*)() const);

    void registerNoteInputAction(const kfw::actions::ActionCode&, NoteInputMethod inputMethod);

    bool noteInputActionAllowed() const;
    void registerNoteAction(const kfw::actions::ActionCode&, NoteName, NoteAddingMode addingMode = NoteAddingMode::NextChord);

    void registerPadNoteAction(const kfw::actions::ActionCode&, Pad padding);
    void registerTabPadNoteAction(const kfw::actions::ActionCode&, Pad padding);

    enum PlayMode {
        NoPlay, PlayNote, PlayChord
    };

    void registerMoveSelectionAction(const kfw::actions::ActionCode& code, MoveSelectionType type, MoveDirection direction,
                                     PlayMode playMode = PlayMode::NoPlay);

    void registerAction(const kfw::actions::ActionCode&, void (INotationInteraction::*)(), bool (NotationActionController::*)() const);
    void registerAction(const kfw::actions::ActionCode&, void (INotationInteraction::*)(), PlayMode = PlayMode::NoPlay,
                        bool (NotationActionController::*)() const = &NotationActionController::isNotationPage);
    template<typename P1>
    void registerAction(const kfw::actions::ActionCode&, void (INotationInteraction::*)(P1), P1, PlayMode = PlayMode::NoPlay,
                        bool (NotationActionController::*)() const = &NotationActionController::isNotationPage);
    template<typename P1>
    void registerAction(const kfw::actions::ActionCode&, void (INotationInteraction::*)(P1), P1,
                        bool (NotationActionController::*)() const);
    template<typename P1, typename P2, typename Q1, typename Q2>
    void registerAction(const kfw::actions::ActionCode&, void (INotationInteraction::*)(P1, P2), Q1, Q2, PlayMode = PlayMode::NoPlay,
                        bool (NotationActionController::*)() const = &NotationActionController::isNotationPage);

    kfw::async::Notification m_currentNotationNoteInputChanged;

    using IsActionEnabledFunc = std::function<bool ()>;
    std::map<kfw::actions::ActionCode, IsActionEnabledFunc> m_isEnabledMap;
};
}
