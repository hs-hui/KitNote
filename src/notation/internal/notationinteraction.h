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

#include <memory>
#include <vector>

#include "modularity/ioc.h"
#include "async/asyncable.h"
#include "iinteractive.h"
#include "engraving/rendering/isinglerenderer.h"
#include "engraving/rendering/ieditmoderenderer.h"

#include "../inotationinteraction.h"
#include "../inotationconfiguration.h"
#include "notationscene/iselectinstrumentscenario.h"
#include "inotationundostack.h"

#include "scoreerrorscontroller.h"

#include "engraving/dom/engravingitem.h"
#include "engraving/dom/elementgroup.h"
#include "engraving/rendering/paintoptions.h"
#include "engraving/types/symid.h"
#include "scorecallbacks.h"

namespace kn::engraving {
class Lasso;
}

class QDrag;

namespace kn::notation {
class Notation;
class NotationSelection;
class NotationSelectionFilter;
class NotationInteraction : public INotationInteraction, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<INotationConfiguration> configuration;
    kfw::ContextInject<ISelectInstrumentsScenario> selectInstrumentScenario = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<engraving::rendering::ISingleRenderer> engravingRenderer = { this };
    kfw::ContextInject<engraving::rendering::IEditModeRenderer> editModeRenderer = { this };

public:
    NotationInteraction(Notation* notation, INotationUndoStackPtr undoStack);

    void paint(kfw::draw::Painter* painter, const engraving::rendering::PaintOptions& opt);

    // Put notes
    INotationNoteInputPtr noteInput() const override;

    // Shadow note
    kn::engraving::ShadowNote* shadowNote() const override;
    void showShadowNoteForPosition(const kfw::PointF& pos) override;
    void showShadowNoteForMidiPitch(const uint8_t pitch) override;
    void hideShadowNote() override;
    kfw::RectF shadowNoteRect() const override;
    kfw::async::Channel</*visible*/ bool> shadowNoteChanged() const override;

    // Visibility
    void toggleVisible() override;

    // Hit
    EngravingItem* hitElement(const kfw::PointF& pos, float width) const override;
    std::vector<EngravingItem*> hitElements(const kfw::PointF& pos, float width) const override;
    Staff* hitStaff(const kfw::PointF& pos) const override;
    const HitElementContext& hitElementContext() const override;
    void setHitElementContext(const HitElementContext& context) override;

    // Select
    void moveChordNoteSelection(MoveDirection d) override;
    void select(const std::vector<EngravingItem*>& elements, SelectType type = SelectType::REPLACE,
                engraving::staff_idx_t staffIndex = 0) override;
    void selectAndStartEditIfNeeded(EngravingItem* element) override;
    void selectAll() override;
    void selectSection() override;
    void selectFirstElement(bool frame = false) override;
    void selectLastElement() override;
    INotationSelectionPtr selection() const override;
    void clearSelection() override;
    kfw::async::Notification selectionChanged() const override;
    void selectTopOrBottomOfChord(MoveDirection d) override;
    void findAndSelectChordRest(const Fraction& tick) override;
    void moveSegmentSelection(MoveDirection d) override;

    EngravingItem* contextItem() const override;

    // SelectionFilter
    INotationSelectionFilterPtr selectionFilter() const override;

    // Drag
    bool isDragStarted() const override;
    void startDrag(const std::vector<EngravingItem*>& elems, const kfw::PointF& eoffset, const IsDraggable& isDraggable) override;
    void drag(const kfw::PointF& fromPos, const kfw::PointF& toPos, DragMode mode) override;
    void endDrag() override;
    kfw::async::Notification dragChanged() const override;

    bool isOutgoingDragElementAllowed(const EngravingItem* element) const override;
    void startOutgoingDragElement(const EngravingItem* element, QObject* dragSource, const kfw::PointF& hotSpot) override;
    void startOutgoingDragRange(QObject* dragSource) override;
    bool isOutgoingDragStarted() const override;
    void endOutgoingDrag() override;

    // Drop
    bool startDropSingle(const QByteArray& edata) override;
    bool startDropRange(const QByteArray& data) override;
    bool startDropRange(const Fraction& sourceTick, const Fraction& tickLength, engraving::staff_idx_t sourceStaffIdx, size_t numStaves,
                        bool preserveMeasureAlignment) override;
    bool startDropImage(const QUrl& url) override;
    bool updateDropSingle(const kfw::PointF& pos, Qt::KeyboardModifiers modifiers) override;
    bool updateDropRange(const kfw::PointF& pos, std::optional<bool> preserveMeasureAlignment = std::nullopt) override;
    bool dropSingle(const kfw::PointF& pos, Qt::KeyboardModifiers modifiers) override;
    bool dropRange(const QByteArray& data, const kfw::PointF& pos, bool deleteSourceMaterial) override;
    void setDropTarget(EngravingItem* item, bool notify = true) override;
    void setDropRect(const kfw::RectF& rect) override;
    void endDrop() override;
    kfw::async::Notification dropChanged() const override;

    bool applyPaletteElement(kn::engraving::EngravingItem* element, Qt::KeyboardModifiers modifiers = {}) override;
    void undo() override;
    void redo() override;
    void undoRedoToIndex(size_t idx) override;

    // Change selection
    bool moveSelectionAvailable(MoveSelectionType type) const override;
    void moveSelection(MoveDirection d, MoveSelectionType type) override;
    void expandSelection(ExpandSelectionMode mode) override;
    void addToSelection(MoveDirection d, MoveSelectionType type) override;
    void selectTopStaff() override;
    void selectEmptyTrailingMeasure() override;

    // Move
    void movePitch(MoveDirection d, PitchMode mode) override;
    void nudge(MoveDirection d, bool quickly) override;
    void nudgeAnchors(MoveDirection) override;
    void moveChordRestToStaff(MoveDirection d) override;
    void moveLyrics(MoveDirection d) override;
    void swapChordRest(MoveDirection d) override;
    void toggleSnapToPrevious() override;
    void toggleSnapToNext() override;

    // Text edit
    bool isTextSelected() const override;
    bool isTextEditingStarted() const override;
    bool textEditingAllowed(const EngravingItem* element) const override;
    void startEditText(EngravingItem* element, const kfw::PointF& cursorPos = kfw::PointF()) override;
    void editText(QInputMethodEvent* event) override;
    void endEditText() override;
    void changeTextCursorPosition(const kfw::PointF& newCursorPos) override;
    void selectText(kn::engraving::SelectTextType type) override;
    const TextBase* editedText() const override;
    kfw::async::Notification textEditingStarted() const override;
    kfw::async::Notification textEditingChanged() const override;
    kfw::async::Channel<TextBase*> textEditingEnded() const override;

    // Grip edit
    bool isGripEditStarted() const override;
    bool isHitGrip(const kfw::PointF& pos) const override;
    void startEditGrip(const kfw::PointF& pos) override;
    void startEditGrip(EngravingItem* element, kn::engraving::Grip grip) override;
    void endEditGrip() override;

    bool isEditingElement() const override;
    kfw::async::Notification isEditingElementChanged() const override;
    void startEditElement(EngravingItem* element) override;
    void changeEditElement(EngravingItem* newElement) override;
    bool isEditAllowed(QKeyEvent* event) override;
    void editElement(QKeyEvent* event) override;
    void endEditElement() override;

    // Anchors edit
    void updateTimeTickAnchors(QKeyEvent* event) override;
    void moveElementAnchors(QKeyEvent* event) override;

    // Measure
    void splitSelectedMeasure() override;
    void joinSelectedMeasures() override;

    void addBoxes(BoxType boxType, int count, AddBoxesTarget target) override;
    void addBoxes(BoxType boxType, int count, int beforeBoxIndex, bool moveSignaturesClef = true) override;

    void copySelection() override;
    void copyLyrics() override;
    void repeatSelection() override;
    void pasteSelection(const Fraction& scale = Fraction(1, 1)) override;
    void swapSelection() override;
    void deleteSelection() override;
    void flipSelection() override;
    void flipSelectionHorizontally() override;
    void addTieToSelection() override;
    void addLaissezVibToSelection() override;
    void addTiedNoteToChord() override;
    void addSlurToSelection() override;
    void addHammerOnPullOffToSelection() override;
    void addOttavaToSelection(OttavaType type) override;
    void addHairpinOnGripDrag(engraving::EditData& ed, bool isLeftGrip) override;
    void addHairpinsToSelection(HairpinType type) override;
    void putRestToSelection() override;
    void putRest(Duration duration) override;
    void addBracketsToSelection(BracketsType type) override;
    void toggleAccidentalForSelection(AccidentalType type) override;
    void toggleArticulationForSelection(SymbolId articulationSymbolId) override;
    void toggleDotsForSelection(Pad dots) override;
    void addGraceNotesToSelectedNotes(GraceNoteType type) override;
    bool canAddTupletToSelectedChordRests() const override;
    void addTupletToSelectedChordRests(const TupletOptions& options) override;
    void addBeamToSelectedChordRests(BeamMode mode) override;

    void increaseDecreaseDuration(int steps, bool stepByDots) override;

    void autoFlipHairpinsType(engraving::Dynamic* selDyn) override;

    void toggleDynamicPopup() override;
    bool toggleLayoutBreakAvailable() const override;
    void toggleLayoutBreak(LayoutBreakType breakType) override;
    void moveMeasureToPrevSystem() override;
    void moveMeasureToNextSystem() override;
    void toggleSystemLock() override;
    void toggleScoreLock() override;
    void makeIntoSystem() override;
    void applySystemLock() override;

    void addRemoveSystemLocks(AddRemoveSystemLockType intervalType, int interval = 0) override;
    bool transpose(const TransposeOptions& options) override;
    void swapVoices(voice_idx_t voiceIndex1, voice_idx_t voiceIndex2) override;
    void addIntervalToSelectedNotes(int interval) override;
    void addFret(int fretIndex) override;
    void changeSelectedElementsVoice(voice_idx_t voiceIndex) override;
    void changeSelectedElementsVoiceAssignment(VoiceAssignment voiceAssignment) override;
    void addAnchoredLineToSelectedNotes() override;

    void addTextToTopFrame(TextStyleType type) override;
    void addTextToItem(TextStyleType type, EngravingItem* item) override;

    kfw::Ret canAddImageToItem(const EngravingItem* item) const override;
    void addImageToItem(const kfw::io::path_t& imagePath, EngravingItem* item) override;

    void addFiguredBass() override;

    void addStretch(qreal value) override;

    Measure* selectedMeasure() const override;
    void addTimeSignature(Measure* measure, engraving::staff_idx_t staffIndex, TimeSignature* timeSignature) override;

    void explodeSelectedStaff() override;
    void implodeSelectedStaff() override;

    void realizeSelectedChordSymbols(bool literal, Voicing voicing, HarmonyDurationType durationType) override;
    void extendToNextNote() override;
    void removeSelectedMeasures() override;
    void removeSelectedRange() override;
    void removeEmptyTrailingMeasures() override;

    void fillSelectionWithSlashes() override;
    void replaceSelectedNotesWithSlashes() override;
    void fillSelectionWithDrumPattern(DrumPattern pattern) override;
    void addAlternatingSticking(bool startWithRight) override;
    void addBeatCounting() override;
    void transformDrumSelection(DrumTransform transform) override;
    void replaceRepeatedMeasuresWithRepeatSigns() override;
    void writeOutMeasureRepeatSigns() override;
    void insertDrumKey() override;
    void removeDrumKey() override;
    DrumStepGrid drumStepGrid() const override;
    QString drumTabText(bool wholeScore = false) const override;
    QString drumTabOfMeasures(kn::engraving::Measure* first, kn::engraving::Measure* last, engraving::staff_idx_t staffIdx) const;
    void copyDrumTab() override;
    QString selectedMeasureDrumTab() const override;
    int fillSelectionWithDrumTab(const QString& measureTab) override;
    int writeDrumTab(const QString& text, int* skippedMeasures, const Fraction& patternLength);
    void pasteDrumTab() override;
    int pasteDrumTabText(const QString& text, int* skippedMeasures = nullptr) override;
    QString drumTabProblem(const QString& text) const override;
    std::vector<ChartSection> chartSections() const override;
    void goToChartSection(int index) override;
    void addChartSection(const QString& name) override;
    void setMusicTempo(int quartersPerMinute) override;
    void setTempoAtMeasure(int quartersPerMinute, Measure* measure) override;
    QString addRepeat(Measure* first, Measure* last, int playCount) override;
    QString addVolta(Measure* first, Measure* last, const std::vector<int>& endings) override;
    void removeRepeats(Measure* first, Measure* last) override;
    void fillChartSection(DrumPattern pattern, bool fillAtEnd) override;
    void toggleDrumStep(int pitch, int step) override;
    void setDrumStepTriplets(bool triplets) override;
    void cycleDrumStepKind(int pitch, int step) override;
    void changeDrumStep(int pitch, int step, bool cycleKind);
    bool m_drumStepTriplets = false;     // KitNote: an empty measure is offered as a grid of triplets
    void selectNextDrumStepMeasure(bool forward) override;
    void changeEnharmonicSpelling(bool) override;
    void spellPitches() override;
    void spellPitchesWithSharps() override;
    void spellPitchesWithFlats() override;
    void regroupNotesAndRests() override;
    void resequenceRehearsalMarks() override;

    void resetStretch() override;
    void resetTextStyleOverrides() override;
    void resetBeamMode() override;
    void resetShapesAndPosition() override;
    void resetToDefaultLayout() override;

    ScoreConfig scoreConfig() const override;
    void setScoreConfig(const ScoreConfig& config) override;
    kfw::async::Channel<ScoreConfigType> scoreConfigChanged() const override;

    void navigateToLyrics(MoveDirection direction, bool moveOnly = true) override;
    void navigateToLyricsVerse(MoveDirection direction) override;

    void navigateToNextSyllable() override;

    void navigateToNearHarmony(MoveDirection direction, bool nearNoteOrRest) override;
    void navigateToHarmonyInNearMeasure(MoveDirection direction) override;
    void navigateToHarmony(const Fraction& ticks) override;

    void navigateToNearFiguredBass(MoveDirection direction) override;
    void navigateToFiguredBassInNearMeasure(MoveDirection direction) override;
    void navigateToFiguredBass(const Fraction& ticks) override;

    void navigateToNearText(MoveDirection direction) override;

    void addMelisma() override;
    void addLyricsVerse() override;

    void addGuitarBend(GuitarBendType bendType) override;
    void addFretboardDiagram() override;

    void toggleBold() override;
    void toggleItalic() override;
    void toggleUnderline() override;
    void toggleStrike() override;
    void toggleSubScript() override;
    void toggleSuperScript() override;
    void toggleArticulation(kn::engraving::SymId) override;
    void toggleOrnament(kn::engraving::SymId) override;
    void toggleAutoplace(bool) override;

    bool canInsertClef(kn::engraving::ClefType) const override;
    void insertClef(kn::engraving::ClefType) override;

    void changeAccidental(kn::engraving::AccidentalType) override;
    void transposeSemitone(int) override;
    void transposeDiatonicAlterations(kn::engraving::TransposeDirection) override;
    void getLocation() override;
    void execute(void (kn::engraving::Score::*)(), const kfw::TranslatableString& actionName) override;

    void showItem(const kn::engraving::EngravingItem* item, int staffIndex = -1) override;
    kfw::async::Channel<ShowItemRequest> showItemRequested() const override;

    void setGetViewRectFunc(const std::function<kfw::RectF()>& func) override;

    void checkAndShowError() override;

    void toggleDebugShowGapRests() override;

private:
    kn::engraving::Score* score() const;
    void onScoreInited();
    void onViewModeChanged();

    void startEdit(const kfw::TranslatableString& actionName);
    void apply();
    void rollback();

    struct ShadowNoteParams {
        kn::engraving::TDuration duration;
        kn::engraving::AccidentalType accidentalType = kn::engraving::AccidentalType::NONE;
        std::set<kn::engraving::SymId> articulationIds;
        kn::engraving::Position position;
    };

    bool doShowShadowNote(kn::engraving::ShadowNote& note, ShadowNoteParams& params);

    bool needStartEditGrip(QKeyEvent* event) const;
    bool handleKeyPress(QKeyEvent* event);
    bool doTextEdit(QKeyEvent* event, TextBase* tb);

    void doEndEditElement();
    void doEndDrag();

    //! NOTE: Helper methods for applyPaletteElement
    void applyPaletteElementToList(EngravingItem* element, kn::engraving::Score* score, const kn::engraving::Selection& sel,
                                   Qt::KeyboardModifiers modifiers = {});
    void applyPaletteElementToRange(EngravingItem* element, kn::engraving::Score* score, const kn::engraving::Selection& sel,
                                    Qt::KeyboardModifiers modifiers = {});

    bool doDropStandard();
    bool doDropTextBaseAndSymbols(const kfw::PointF& pos, bool applyUserOffset);

    void onElementDestroyed(EngravingItem* element);

    void doSelect(const std::vector<EngravingItem*>& elements, SelectType type, engraving::staff_idx_t staffIndex = 0);
    void selectElementsWithSameTypeOnSegment(kn::engraving::ElementType elementType, kn::engraving::Segment* segment);

    void notifyAboutDragChanged();
    void notifyAboutDropChanged();
    void notifyAboutSelectionChangedIfNeed();
    void notifyAboutNotationChanged();
    void notifyAboutTextEditingStarted();
    void notifyAboutTextEditingChanged();
    void notifyAboutTextEditingEnded(TextBase* text);
    void notifyAboutNoteInputStateChanged();
    void doDragLasso(const kfw::PointF& p);
    void endLasso();
    void toggleFontStyle(kn::engraving::FontStyle);
    void toggleVerticalAlignment(kn::engraving::VerticalAlignment);
    void navigateToLyrics(bool, bool, bool);

    Harmony* editedHarmony() const;
    Segment* harmonySegment(const Harmony* harmony) const;
    kn::engraving::Harmony* findHarmonyInSegment(const kn::engraving::Segment* segment, engraving::track_idx_t track,
                                                 kn::engraving::TextStyleType textStyleType) const;
    kn::engraving::Harmony* createHarmony(kn::engraving::Segment* segment, engraving::track_idx_t track,
                                          kn::engraving::HarmonyType type) const;

    bool canAddTextToItem(TextStyleType type, const EngravingItem* item) const;

    void addText(TextStyleType type, EngravingItem* item = nullptr);

    void startEditText(kn::engraving::TextBase* text);
    bool needEndTextEdit() const;
    void pasteIntoTextEdit();

    kn::engraving::Page* point2page(const kfw::PointF& p, bool useNearestPage = false) const;
    std::vector<EngravingItem*> elementsAt(const kfw::PointF& p) const;
    EngravingItem* elementAt(const kfw::PointF& p) const;

    // Sorting using this function will place the elements that are the most
    // interesting to be selected at the end of the list
    static bool elementIsLess(const kn::engraving::EngravingItem* e1, const kn::engraving::EngravingItem* e2);

    void updateGripAnchorLines();
    void updateDragAnchorLines();
    void setAnchorLines(const std::vector<kfw::LineF>& anchorList);
    void resetAnchorLines();

    double getHRaster() const;
    double getVRaster() const;

    double currentScaling(kfw::draw::Painter* painter) const;

    std::vector<ShadowNoteParams> previewNotes() const;

    bool shouldDrawInputPreview() const;
    void drawInputPreview(kfw::draw::Painter* painter, const engraving::rendering::PaintOptions& opt);

    void drawAnchorLines(kfw::draw::Painter* painter);
    void drawTextEditMode(kfw::draw::Painter* painter, const engraving::rendering::PaintOptions& opt);
    void drawSelectionRange(kfw::draw::Painter* painter);
    void drawGripPoints(kfw::draw::Painter* painter, const engraving::rendering::PaintOptions& opt);
    void drawLasso(kfw::draw::Painter* painter, const engraving::rendering::PaintOptions& opt);
    void drawDrop(kfw::draw::Painter* painter);

    void moveElementSelection(MoveDirection d);
    void moveStringSelection(MoveDirection d);

    EngravingItem* dropTarget(kn::engraving::EditData& ed) const;
    bool prepareDropStandardElement(const kfw::PointF& pos, Qt::KeyboardModifiers modifiers);
    bool prepareDropMeasureAnchorElement(const kfw::PointF& pos);
    bool prepareDropTimeAnchorElement(const kfw::PointF& pos);
    bool dropCanvas(EngravingItem* e);
    void resetDropData();

    void doFinishAddFretboardDiagram();

    bool selectInstrument(kn::engraving::InstrumentChange* instrumentChange);
    void cleanupDrumsetChanges(kn::engraving::InstrumentChange* instrumentChange) const;

    void applyDropPaletteElement(kn::engraving::Score* score, kn::engraving::EngravingItem* target, kn::engraving::EngravingItem* e,
                                 Qt::KeyboardModifiers modifiers, kn::engraving::track_idx_t track = kfw::nidx);

    void applyLineNoteToNote(engraving::Score* score, Note* note1, Note* note2, EngravingItem* line);

    void doAddSlur(const kn::engraving::Slur* slurTemplate = nullptr);
    void doAddSlur(EngravingItem* firstItem, EngravingItem* secondItem, const kn::engraving::Slur* slurTemplate);

    bool scoreHasMeasure() const;
    bool notesHaveActiculation(const std::vector<Note*>& notes, SymbolId articulationSymbolId) const;

    bool needEndTextEditing(const std::vector<EngravingItem*>& newSelectedElements) const;
    bool needEndElementEditing(const std::vector<EngravingItem*>& newSelectedElements) const;

    void resetGripEdit();
    void resetHitElementContext();

    template<typename P>
    void execute(void (kn::engraving::Score::* function)(P), P param, const kfw::TranslatableString& actionName);

    struct HitMeasureData
    {
        Measure* measure = nullptr;
        Staff* staff = nullptr;
    };

    HitMeasureData hitMeasure(const kfw::PointF& pos) const;

    struct DragData
    {
        kfw::PointF beginMove;
        kfw::PointF elementOffset;
        kn::engraving::EditData ed;
        std::vector<EngravingItem*> elements;
        std::vector<std::unique_ptr<kn::engraving::ElementGroup> > dragGroups;
        void reset();
    };

    struct ElementDropData
    {
        kn::engraving::EditData ed;
        EngravingItem* dropTarget = nullptr;
        kfw::RectF dropRect;
    };

    struct RangeDropData
    {
        engraving::Fraction sourceTick;
        engraving::staff_idx_t sourceStaffIdx = kfw::nidx;

        engraving::Fraction tickLength;
        engraving::Fraction timeStretch;
        size_t numStaves = 0;

        bool preserveMeasureAlignment = false;

        engraving::Segment* targetSegment = nullptr;
        engraving::staff_idx_t targetStaffIdx = 0;

        std::vector<kfw::RectF> dropRects;
    };

    struct DropData
    {
        std::optional<ElementDropData> elementDropData;
        std::optional<RangeDropData> rangeDropData;
    };

    ScoreCallbacks m_scoreCallbacks;
    Notation* m_notation = nullptr;
    INotationUndoStackPtr m_undoStack;

    INotationNoteInputPtr m_noteInput = nullptr;

    kfw::async::Channel</*visible*/ bool> m_shadowNoteChanged;

    std::shared_ptr<NotationSelection> m_selection = nullptr;
    kfw::async::Notification m_selectionChanged;

    std::shared_ptr<NotationSelectionFilter> m_selectionFilter = nullptr;

    std::shared_ptr<ScoreErrorsController> m_errorsController = nullptr;

    DragData m_dragData;
    kfw::async::Notification m_dragChanged;
    std::vector<kfw::LineF> m_anchorLines;

    QDrag* m_outgoingDrag = nullptr;

    kn::engraving::EditData m_editData;

    kfw::async::Notification m_isEditingElementChanged;

    kfw::async::Notification m_textEditingStarted;
    kfw::async::Notification m_textEditingChanged;
    kfw::async::Channel<TextBase*> m_textEditingEnded;
    kfw::async::Channel<TextBase*> m_textAdded;

    DropData m_dropData;
    kfw::async::Notification m_dropChanged;

    kfw::async::Channel<ScoreConfigType> m_scoreConfigChanged;

    engraving::BspTree m_droppableTree;
    Page* m_currentDropPage = nullptr;

    kn::engraving::Lasso* m_lasso = nullptr;

    bool m_notifyAboutDropChanged = false;
    HitElementContext m_hitElementContext;

    kfw::async::Channel<ShowItemRequest> m_showItemRequested;
};
}
