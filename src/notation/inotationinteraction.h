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

#include <functional>

#include "async/notification.h"

#include "notationtypes.h"
#include "inotationnoteinput.h"
#include "inotationselection.h"
#include "inotationselectionfilter.h"

class QKeyEvent;
class QInputMethodEvent;

namespace kn::notation {
class INotationInteraction
{
public:
    virtual ~INotationInteraction() = default;

    // Put notes
    virtual INotationNoteInputPtr noteInput() const = 0;

    // Shadow note
    virtual kn::engraving::ShadowNote* shadowNote() const = 0;
    virtual void showShadowNoteForPosition(const kfw::PointF& pos) = 0;
    virtual void showShadowNoteForMidiPitch(const uint8_t pitch) = 0;
    virtual void hideShadowNote() = 0;
    virtual kfw::RectF shadowNoteRect() const = 0;
    virtual kfw::async::Channel</*visible*/ bool> shadowNoteChanged() const = 0;

    // Visibility
    virtual void toggleVisible() = 0;

    // Hit
    virtual EngravingItem* hitElement(const kfw::PointF& pos, float width) const = 0;
    virtual std::vector<EngravingItem*> hitElements(const kfw::PointF& pos, float width) const = 0;
    virtual Staff* hitStaff(const kfw::PointF& pos) const = 0;

    struct HitElementContext
    {
        notation::EngravingItem* element = nullptr;
        notation::Staff* staff = nullptr;

        bool operator ==(const HitElementContext& other) const
        {
            return element == other.element && staff == other.staff;
        }
    };

    virtual const HitElementContext& hitElementContext() const = 0;
    virtual void setHitElementContext(const HitElementContext& context) = 0;

    // Select
    virtual void moveChordNoteSelection(MoveDirection d) = 0;
    virtual void select(const std::vector<EngravingItem*>& elements, SelectType type = SelectType::REPLACE,
                        engraving::staff_idx_t staffIndex = 0) = 0;
    virtual void selectAndStartEditIfNeeded(EngravingItem* element) = 0;
    virtual void selectAll() = 0;
    virtual void selectSection() = 0;
    virtual void selectFirstElement(bool frame = true) = 0;
    virtual void selectLastElement() = 0;
    virtual INotationSelectionPtr selection() const = 0;
    virtual void clearSelection() = 0;
    virtual kfw::async::Notification selectionChanged() const = 0;
    virtual void selectTopOrBottomOfChord(MoveDirection d) = 0;
    virtual void findAndSelectChordRest(const Fraction& tick) = 0;

    virtual EngravingItem* contextItem() const = 0;

    // SelectionFilter
    virtual INotationSelectionFilterPtr selectionFilter() const = 0;

    // Drag
    using IsDraggable = std::function<bool (const EngravingItem*)>;
    virtual bool isDragStarted() const = 0;
    virtual void startDrag(const std::vector<EngravingItem*>& elems, const kfw::PointF& eoffset, const IsDraggable& isDrag) = 0;
    virtual void drag(const kfw::PointF& fromPos, const kfw::PointF& toPos, DragMode mode) = 0;
    virtual void endDrag() = 0;
    virtual kfw::async::Notification dragChanged() const = 0;

    virtual bool isOutgoingDragElementAllowed(const EngravingItem* element) const = 0;
    virtual void startOutgoingDragElement(const EngravingItem* element, QObject* dragSource, const kfw::PointF& hotSpot) = 0;
    virtual void startOutgoingDragRange(QObject* dragSource) = 0;
    virtual bool isOutgoingDragStarted() const = 0;
    virtual void endOutgoingDrag() = 0;

    // Drop
    //! TODO Change KeyboardModifiers to modes
    virtual bool startDropSingle(const QByteArray& edata) = 0;
    virtual bool startDropRange(const QByteArray& data) = 0;
    virtual bool startDropRange(const Fraction& sourceTick, const Fraction& tickLength, engraving::staff_idx_t sourceStaffIdx,
                                size_t numStaves, bool preserveMeasureAlignment) = 0;
    virtual bool startDropImage(const QUrl& url) = 0;
    virtual bool updateDropSingle(const kfw::PointF& pos, Qt::KeyboardModifiers modifiers) = 0; //! NOTE Also may set drop target
    virtual bool updateDropRange(const kfw::PointF& pos, std::optional<bool> preserveMeasureAlignment = std::nullopt) = 0;
    virtual bool dropSingle(const kfw::PointF& pos, Qt::KeyboardModifiers modifiers) = 0;
    virtual bool dropRange(const QByteArray& data, const kfw::PointF& pos, bool deleteSourceMaterial) = 0;
    virtual void setDropTarget(EngravingItem* item, bool notify = true) = 0;
    virtual void setDropRect(const kfw::RectF& rect) = 0;
    virtual void endDrop() = 0;
    virtual kfw::async::Notification dropChanged() const = 0;

    virtual bool applyPaletteElement(kn::engraving::EngravingItem* element, Qt::KeyboardModifiers modifiers = {}) = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual void undoRedoToIndex(size_t idx) = 0;

    // Change selection
    virtual bool moveSelectionAvailable(MoveSelectionType type) const = 0;
    virtual void moveSelection(MoveDirection d, MoveSelectionType type) = 0;

    virtual void moveLyrics(MoveDirection d) = 0;
    virtual void expandSelection(ExpandSelectionMode mode) = 0;
    virtual void addToSelection(MoveDirection d, MoveSelectionType type) = 0;
    virtual void selectTopStaff() = 0;
    virtual void selectEmptyTrailingMeasure() = 0;
    virtual void moveSegmentSelection(MoveDirection d) = 0;

    // Move/nudge elements
    virtual void movePitch(MoveDirection d, PitchMode mode) = 0;
    virtual void nudge(MoveDirection d, bool quickly) = 0;
    virtual void nudgeAnchors(MoveDirection d) = 0;
    virtual void moveChordRestToStaff(MoveDirection d) = 0;
    virtual void swapChordRest(MoveDirection d) = 0;
    virtual void toggleSnapToPrevious() = 0;
    virtual void toggleSnapToNext() = 0;

    // Text edit
    virtual bool isTextSelected() const = 0;
    virtual bool isTextEditingStarted() const = 0;
    virtual bool textEditingAllowed(const EngravingItem* element) const = 0;
    virtual void startEditText(EngravingItem* element, const kfw::PointF& elementPos = kfw::PointF()) = 0;
    virtual void editText(QInputMethodEvent* event) = 0;
    virtual void endEditText() = 0;
    virtual void changeTextCursorPosition(const kfw::PointF& newCursorPos) = 0;
    virtual void selectText(kn::engraving::SelectTextType type) = 0;
    virtual const TextBase* editedText() const = 0;
    virtual kfw::async::Notification textEditingStarted() const = 0;
    virtual kfw::async::Notification textEditingChanged() const = 0;
    virtual kfw::async::Channel<TextBase*> textEditingEnded() const = 0;

    // Display
    virtual kfw::async::Channel<ScoreConfigType> scoreConfigChanged() const = 0;

    // Grip edit
    virtual bool isGripEditStarted() const = 0;
    virtual bool isHitGrip(const kfw::PointF& pos) const = 0;
    virtual void startEditGrip(const kfw::PointF& pos) = 0;
    virtual void startEditGrip(EngravingItem* element, kn::engraving::Grip grip) = 0;
    virtual void endEditGrip() = 0;

    virtual bool isEditingElement() const = 0;
    virtual kfw::async::Notification isEditingElementChanged() const = 0;
    virtual void startEditElement(EngravingItem* element) = 0;
    virtual void changeEditElement(EngravingItem* newElement) = 0;
    virtual bool isEditAllowed(QKeyEvent* event) = 0;
    virtual void editElement(QKeyEvent* event) = 0;
    virtual void endEditElement() = 0;

    // Anchors edit
    virtual void updateTimeTickAnchors(QKeyEvent* event) = 0;
    virtual void moveElementAnchors(QKeyEvent* event) = 0;

    virtual void splitSelectedMeasure() = 0;
    virtual void joinSelectedMeasures() = 0;

    virtual void addBoxes(BoxType boxType, int count, AddBoxesTarget target) = 0;
    virtual void addBoxes(BoxType boxType, int count, int beforeBoxIndex, bool insertAfter) = 0;

    virtual void copySelection() = 0;
    virtual void repeatSelection() = 0;
    virtual void copyLyrics() = 0;
    virtual void pasteSelection(const Fraction& scale = Fraction(1, 1)) = 0;
    virtual void swapSelection() = 0;
    virtual void deleteSelection() = 0;
    virtual void flipSelection() = 0;
    virtual void flipSelectionHorizontally() = 0;
    virtual void addTieToSelection() = 0;
    virtual void addTiedNoteToChord() = 0;
    virtual void addLaissezVibToSelection() = 0;
    virtual void addSlurToSelection() = 0;
    virtual void addHammerOnPullOffToSelection() = 0;
    virtual void addOttavaToSelection(OttavaType type) = 0;
    virtual void addHairpinOnGripDrag(engraving::EditData& ed, bool isLeftGrip) = 0;
    virtual void addHairpinsToSelection(HairpinType type) = 0;
    virtual void putRestToSelection() = 0;
    virtual void putRest(Duration duration) = 0;
    virtual void addBracketsToSelection(BracketsType type) = 0;
    virtual void toggleAccidentalForSelection(AccidentalType type) = 0;
    virtual void toggleArticulationForSelection(SymbolId articulationSymbolId) = 0;
    virtual void toggleDotsForSelection(Pad dots) = 0;
    virtual void addGraceNotesToSelectedNotes(GraceNoteType type) = 0;
    virtual bool canAddTupletToSelectedChordRests() const = 0;
    virtual void addTupletToSelectedChordRests(const TupletOptions& options) = 0;
    virtual void addBeamToSelectedChordRests(BeamMode mode) = 0;

    virtual void increaseDecreaseDuration(int steps, bool stepByDots) = 0;

    virtual void autoFlipHairpinsType(engraving::Dynamic* selDyn) = 0;

    virtual void toggleDynamicPopup() = 0;
    virtual bool toggleLayoutBreakAvailable() const = 0;
    virtual void toggleLayoutBreak(LayoutBreakType breakType) = 0;
    virtual void moveMeasureToPrevSystem() = 0;
    virtual void moveMeasureToNextSystem() = 0;
    virtual void toggleSystemLock() = 0;
    virtual void toggleScoreLock() = 0;
    virtual void makeIntoSystem() = 0;
    virtual void applySystemLock() = 0;

    virtual void addRemoveSystemLocks(AddRemoveSystemLockType intervalType, int interval = 0) = 0;
    virtual bool transpose(const TransposeOptions& options) = 0;
    virtual void swapVoices(voice_idx_t voiceIndex1, voice_idx_t voiceIndex2) = 0;
    virtual void addIntervalToSelectedNotes(int interval) = 0;
    virtual void addFret(int fretIndex) = 0;
    virtual void changeSelectedElementsVoice(voice_idx_t voiceIndex) = 0;
    virtual void changeSelectedElementsVoiceAssignment(VoiceAssignment voiceAssignment) = 0;
    virtual void addAnchoredLineToSelectedNotes() = 0;

    virtual void addTextToTopFrame(TextStyleType type) = 0;
    virtual void addTextToItem(TextStyleType type, EngravingItem* item) = 0;

    virtual kfw::Ret canAddImageToItem(const EngravingItem* item) const = 0;
    virtual void addImageToItem(const kfw::io::path_t& imagePath, EngravingItem* item) = 0;

    virtual void addFiguredBass() = 0;
    virtual void addFretboardDiagram() = 0;

    virtual void addStretch(qreal value) = 0;

    virtual Measure* selectedMeasure() const = 0;
    virtual void addTimeSignature(Measure* measure, engraving::staff_idx_t staffIndex, TimeSignature* timeSignature) = 0;

    virtual void explodeSelectedStaff() = 0;
    virtual void implodeSelectedStaff() = 0;

    virtual void realizeSelectedChordSymbols(bool literal, Voicing voicing, HarmonyDurationType durationType) = 0;
    virtual void extendToNextNote() = 0;
    virtual void removeSelectedMeasures() = 0;
    virtual void removeSelectedRange() = 0;
    virtual void removeEmptyTrailingMeasures() = 0;

    virtual void fillSelectionWithSlashes() = 0;
    virtual void replaceSelectedNotesWithSlashes() = 0;
    virtual void fillSelectionWithDrumPattern(DrumPattern pattern) = 0; // KitNote
    virtual void addAlternatingSticking(bool startWithRight) = 0; // KitNote
    virtual void addBeatCounting() = 0; // KitNote
    virtual void transformDrumSelection(DrumTransform transform) = 0; // KitNote
    virtual void replaceRepeatedMeasuresWithRepeatSigns() = 0; // KitNote
    virtual void writeOutMeasureRepeatSigns() = 0; // KitNote
    virtual void insertDrumKey() = 0; // KitNote
    virtual void removeDrumKey() = 0; // KitNote
    virtual DrumStepGrid drumStepGrid() const = 0; // KitNote
    virtual QString drumTabText(bool wholeScore = false) const = 0; // KitNote
    virtual void copyDrumTab() = 0; // KitNote
    virtual QString selectedMeasureDrumTab() const = 0; // KitNote
    virtual int fillSelectionWithDrumTab(const QString& measureTab) = 0; // KitNote
    virtual void pasteDrumTab() = 0; // KitNote
    virtual int pasteDrumTabText(const QString& text, int* skippedMeasures = nullptr) = 0; // KitNote
    virtual QString drumTabProblem(const QString& text) const = 0; // KitNote
    virtual std::vector<ChartSection> chartSections() const = 0; // KitNote
    virtual void goToChartSection(int index) = 0; // KitNote
    virtual void addChartSection(const QString& name) = 0; // KitNote
    virtual void setMusicTempo(int quartersPerMinute) = 0; // KitNote
    virtual void setTempoAtMeasure(int quartersPerMinute, Measure* measure) = 0; // KitNote
    virtual QString addRepeat(Measure* first, Measure* last, int playCount) = 0; // KitNote
    virtual QString addVolta(Measure* first, Measure* last, const std::vector<int>& endings) = 0; // KitNote
    virtual void removeRepeats(Measure* first, Measure* last) = 0; // KitNote
    virtual void fillChartSection(DrumPattern pattern, bool fillAtEnd) = 0; // KitNote
    virtual void toggleDrumStep(int pitch, int step) = 0; // KitNote
    virtual void setDrumStepTriplets(bool triplets) = 0; // KitNote
    virtual void cycleDrumStepKind(int pitch, int step) = 0; // KitNote
    virtual void selectNextDrumStepMeasure(bool forward) = 0; // KitNote

    virtual void changeEnharmonicSpelling(bool both) = 0;
    virtual void spellPitches() = 0;
    virtual void spellPitchesWithSharps() = 0;
    virtual void spellPitchesWithFlats() = 0;
    virtual void regroupNotesAndRests() = 0;
    virtual void resequenceRehearsalMarks() = 0;

    virtual void resetStretch() = 0;
    virtual void resetTextStyleOverrides() = 0;
    virtual void resetBeamMode() = 0;
    virtual void resetShapesAndPosition() = 0;
    virtual void resetToDefaultLayout() = 0;

    virtual ScoreConfig scoreConfig() const = 0;
    virtual void setScoreConfig(const ScoreConfig& config) = 0;

    virtual void addMelisma() = 0;
    virtual void addLyricsVerse() = 0;

    virtual void addGuitarBend(GuitarBendType bendType) = 0;

    // Text navigation
    virtual void navigateToLyrics(MoveDirection direction, bool moveOnly = false) = 0;
    virtual void navigateToLyricsVerse(MoveDirection direction) = 0;

    virtual void navigateToNextSyllable() = 0;

    virtual void navigateToNearHarmony(MoveDirection direction, bool nearNoteOrRest) = 0;
    virtual void navigateToHarmonyInNearMeasure(MoveDirection direction) = 0;
    virtual void navigateToHarmony(const Fraction& ticks) = 0;

    virtual void navigateToNearFiguredBass(MoveDirection direction) = 0;
    virtual void navigateToFiguredBassInNearMeasure(MoveDirection direction) = 0;
    virtual void navigateToFiguredBass(const Fraction& ticks) = 0;

    virtual void navigateToNearText(MoveDirection direction) = 0;

    // Text style
    virtual void toggleBold() = 0;
    virtual void toggleItalic() = 0;
    virtual void toggleUnderline() = 0;
    virtual void toggleStrike() = 0;
    virtual void toggleSubScript() = 0;
    virtual void toggleSuperScript() = 0;

    virtual bool canInsertClef(kn::engraving::ClefType) const = 0;
    virtual void insertClef(kn::engraving::ClefType) = 0;

    virtual void toggleArticulation(kn::engraving::SymId) = 0;
    virtual void toggleOrnament(kn::engraving::SymId) = 0;
    virtual void changeAccidental(kn::engraving::AccidentalType) = 0;
    virtual void transposeSemitone(int) = 0;
    virtual void transposeDiatonicAlterations(kn::engraving::TransposeDirection) = 0;
    virtual void toggleAutoplace(bool all) = 0;
    virtual void getLocation() = 0;
    virtual void execute(void (kn::engraving::Score::*)(), const kfw::TranslatableString& actionName) = 0;

    struct ShowItemRequest {
        const EngravingItem* item = nullptr;
        kfw::RectF showRect;
    };

    virtual void showItem(const kn::engraving::EngravingItem* item, int staffIndex = -1) = 0;
    virtual kfw::async::Channel<ShowItemRequest> showItemRequested() const = 0;

    virtual void setGetViewRectFunc(const std::function<kfw::RectF()>& func) = 0;

    virtual void checkAndShowError() = 0;

    virtual void toggleDebugShowGapRests() = 0;
};

using INotationInteractionPtr = std::shared_ptr<INotationInteraction>;
}
