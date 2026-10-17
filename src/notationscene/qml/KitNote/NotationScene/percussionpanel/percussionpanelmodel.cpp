/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include "percussionpanelmodel.h"

#include <QCoreApplication>
#include <QKeyEvent>

#include "audio/common/audioutils.h"

#include "notationscene/utilities/percussionutilities.h"

#include "ui/view/iconcodes.h"

#include "engraving/dom/factory.h"
#include "engraving/dom/masterscore.h"
#include "engraving/dom/measure.h"
#include "engraving/dom/segment.h"
#include "engraving/dom/utils.h"
#include "engraving/editing/undo.h"

static const QString PAD_NAMES_CODE("percussion-pad-names");
static const QString NOTATION_PREVIEW_CODE("percussion-notation-preview");
static const QString SET_COLUMNS_CODE("percussion-set-columns");
static const QString EDIT_LAYOUT_CODE("percussion-edit-layout");
static const QString RESET_LAYOUT_CODE("percussion-reset-layout");

using namespace kfw;
using namespace kfw::actions;
using namespace kfw::ui;
using namespace kn::notation;

static const std::unordered_map<PercussionPanelPadModel::PadAction, NoteAddingMode> WRITE_ACTION_MAP = {
    { PercussionPanelPadModel::PadAction::TRIGGER_STANDARD, NoteAddingMode::NextChord },
    { PercussionPanelPadModel::PadAction::TRIGGER_ADD, NoteAddingMode::CurrentChord },
    { PercussionPanelPadModel::PadAction::TRIGGER_INSERT, NoteAddingMode::InsertChord }
};

PercussionPanelModel::PercussionPanelModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
    m_padListModel = new PercussionPanelPadListModel(this);
    qApp->installEventFilter(this);
}

bool PercussionPanelModel::enabled() const
{
    return m_enabled;
}

void PercussionPanelModel::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }
    m_enabled = enabled;
    emit enabledChanged();
}

QString PercussionPanelModel::soundTitle() const
{
    return m_soundTitle;
}

void PercussionPanelModel::setSoundTitle(const QString& soundTitle)
{
    if (m_soundTitle == soundTitle) {
        return;
    }
    m_soundTitle = soundTitle;
    emit soundTitleChanged();
}

PanelMode::Mode PercussionPanelModel::currentPanelMode() const
{
    return m_currentPanelMode;
}

void PercussionPanelModel::setCurrentPanelMode(const PanelMode::Mode& panelMode)
{
    if (m_currentPanelMode == panelMode) {
        return;
    }

    // After editing, return to the last non-edit mode...
    if (panelMode != PanelMode::Mode::EDIT_LAYOUT && m_panelModeToRestore != panelMode) {
        m_panelModeToRestore = panelMode;
    }

    m_currentPanelMode = panelMode;
    emit currentPanelModeChanged(m_currentPanelMode);
}

bool PercussionPanelModel::useNotationPreview() const
{
    return configuration()->percussionPanelUseNotationPreview();
}

void PercussionPanelModel::setUseNotationPreview(bool useNotationPreview)
{
    if (configuration()->percussionPanelUseNotationPreview() == useNotationPreview) {
        return;
    }
    configuration()->setPercussionPanelUseNotationPreview(useNotationPreview);
}

int PercussionPanelModel::notationPreviewNumStaffLines() const
{
    if (!interaction()) {
        return 0;
    }

    const NoteInputState& inputState = interaction()->noteInput()->state();
    const Staff* staff = inputState.staff();

    return staff ? staff->lines(inputState.tick()) : 0;
}

QColor PercussionPanelModel::notationPreviewBackgroundColor() const
{
    const kfw::Color color = engravingConfiguration()->noteBackgroundColor();
    return color.toQColor();
}

PercussionPanelPadListModel* PercussionPanelModel::padListModel() const
{
    return m_padListModel;
}

void PercussionPanelModel::init()
{
    setEnabled(m_padListModel->hasActivePads());

    m_padListModel->hasActivePadsChanged().onNotify(this, [this]() {
        setEnabled(m_padListModel->hasActivePads());
    });

    m_padListModel->padActionRequested().onReceive(this, [this](PercussionPanelPadModel::PadAction action, int pitch) {
        switch (action) {
            case PercussionPanelPadModel::PadAction::TRIGGER_STANDARD:
            case PercussionPanelPadModel::PadAction::TRIGGER_ADD:
            case PercussionPanelPadModel::PadAction::TRIGGER_INSERT:
                onPadTriggered(pitch, action);
                break;
            case PercussionPanelPadModel::PadAction::DUPLICATE:
                onDuplicatePadRequested(pitch);
                break;
            case PercussionPanelPadModel::PadAction::DELETE:
                onDeletePadRequested(pitch);
                break;
            case PercussionPanelPadModel::PadAction::DEFINE_SHORTCUT:
                onDefinePadShortcutRequested(pitch);
                break;
            default:
                break;
        }
    });

    configuration()->percussionPanelUseNotationPreviewChanged().onNotify(this, [this]() {
        const bool useNotationPreview = configuration()->percussionPanelUseNotationPreview();
        emit useNotationPreviewChanged(useNotationPreview);
    });

    onCurrentNotationChanged();

    globalContext()->currentNotationChanged().onNotify(this, [this] {
        onCurrentNotationChanged();
    });
}

QList<QVariantMap> PercussionPanelModel::layoutMenuItems() const
{
    const auto editLayoutTitle = m_currentPanelMode == PanelMode::Mode::EDIT_LAYOUT
                                 ? kfw::qtrc("notation/percussion", "Finish editing")
                                 : kfw::qtrc("notation/percussion", "Edit layout");

    static constexpr int editLayoutIcon = static_cast<int>(IconCode::Code::CONFIGURE);
    static constexpr int resetLayoutIcon = static_cast<int>(IconCode::Code::UNDO);

    const QVariantList columnsSubmenu = {
        createColumnSubItem(4),
        createColumnSubItem(6),
        createColumnSubItem(8),
        createColumnSubItem(12),
        createColumnSubItem(16),
        createColumnSubItem(20),
        createColumnSubItem(24),
    };

    QList<QVariantMap> menuItems = {
        { { "id", PAD_NAMES_CODE }, { "title", kfw::qtrc("notation/percussion", "Pad names") },
            { "checkable", true }, { "checked", !useNotationPreview() }, { "enabled", true } },

        { { "id", NOTATION_PREVIEW_CODE }, { "title", kfw::qtrc("notation/percussion", "Notation preview") },
            { "checkable", true }, { "checked", useNotationPreview() }, { "enabled", true } },

        { }, // separator

        { { "title", kfw::qtrc("notation/percussion",  "%1 columns").arg(m_padListModel->numColumns()) },
            { "subitems", columnsSubmenu }, { "enabled", true } },

        { { "id", EDIT_LAYOUT_CODE },
            { "title", editLayoutTitle }, { "icon", editLayoutIcon }, { "enabled", true } },

        { { "id", RESET_LAYOUT_CODE },
            { "title", kfw::qtrc("notation/percussion", "Reset layout") }, { "icon", resetLayoutIcon }, { "enabled", true } },
    };

    return menuItems;
}

QVariantMap PercussionPanelModel::createColumnSubItem(int numColumns) const
{
    const QString title = QString::number(numColumns);
    const QString id = SET_COLUMNS_CODE + "-" + title;
    return { { "id", id }, { "title", title },
        { "checkable", true }, { "checked", m_padListModel->numColumns() == numColumns }, { "enabled", true } };
}

void PercussionPanelModel::handleMenuItem(const QString& itemId)
{
    if (itemId == PAD_NAMES_CODE) {
        setUseNotationPreview(false);
    } else if (itemId == NOTATION_PREVIEW_CODE) {
        setUseNotationPreview(true);
    } else if (itemId == EDIT_LAYOUT_CODE) {
        if (m_currentPanelMode == PanelMode::Mode::EDIT_LAYOUT) {
            finishEditing();
            m_padListModel->focusFirstActivePad();
            return;
        }
        setCurrentPanelMode(PanelMode::Mode::EDIT_LAYOUT);
        m_padListModel->padFocusRequested(0);
    } else if (itemId == RESET_LAYOUT_CODE) {
        resetLayout();
    } else if (itemId.contains(SET_COLUMNS_CODE)) {
        //! NOTE: In this case, the last section of the itemId refers to the number of columns
        auto stringSection = itemId.section(u'-', -1, -1);
        const int num = stringSection.toInt();
        setColumns(num);
    }
}

void PercussionPanelModel::finishEditing(bool discardChanges)
{
    setCurrentPanelMode(m_panelModeToRestore);

    if (!interaction()) {
        //! NOTE: Can happen if we close the project while editing the layout...
        setDrumset(nullptr);
        return;
    }

    m_padListModel->removeEmptyRows();

    const std::pair<Instrument*, Part*> instAndPart = getCurrentInstrumentAndPart();
    Instrument* inst = instAndPart.first;
    Part* part = instAndPart.second;

    if (discardChanges) {
        setDrumset(inst ? inst->drumset() : nullptr);
        return;
    }

    IF_ASSERT_FAILED(inst && inst->drumset() && part) {
        return;
    }

    Drumset updatedDrumset = *m_padListModel->drumset();

    for (int i = 0; i < m_padListModel->padList().size(); ++i) {
        const PercussionPanelPadModel* model = m_padListModel->padList().at(i);
        if (!model) {
            continue;
        }

        const int row = i / m_padListModel->numColumns();
        const int column = i % m_padListModel->numColumns();

        engraving::DrumInstrument& drum = updatedDrumset.drum(model->pitch());

        drum.panelRow = row;
        drum.panelColumn = column;

        drum.shortcut = model->keyboardShortcut();
    }

    // Return if nothing changed after edit...
    if (*inst->drumset() == updatedDrumset) {
        setCurrentPanelMode(m_panelModeToRestore);
        m_padListModel->focusLastActivePad();
        return;
    }

    const InstrumentKey key = { inst->id(), part->id() };
    notation()->parts()->replaceDrumset(key, updatedDrumset);

    m_padListModel->focusLastActivePad();
}

void PercussionPanelModel::customizeKit()
{
    dispatcher()->dispatch("customize-kit");
}

void PercussionPanelModel::onCurrentNotationChanged()
{
    const auto updatePadModels = [this](Drumset* drumset) {
        emit notationPreviewNumStaffLinesChanged();

        if (drumset && m_padListModel->drumset() && *drumset == *m_padListModel->drumset()) {
            return;
        }

        if (m_currentPanelMode == PanelMode::Mode::EDIT_LAYOUT) {
            finishEditing(/*discardChanges*/ true);
        } else {
            setDrumset(drumset);
        }

        updateSoundTitle(currentTrackId());
    };

    if (!notation()) {
        updatePadModels(nullptr);
        return;
    }

    // KitNote: every staff is a drum staff, so select the first note or rest of a freshly
    // opened score. The panel then shows its pads straight away instead of asking for a selection.
    // Once per notation only: coming back to a score with nothing selected must not move the
    // selection to the first measure behind the user's back.
    const void* current = notation().get();
    if (!kfw::contains(m_autoSelectedNotations, current)) {
        m_autoSelectedNotations.insert(current);

        if (interaction()->selection()->isNone()) {
            // the measure as it is shown: a multimeasure rest, if the score begins with one
            const kn::engraving::Measure* measure = notation()->elements()->msScore()->firstMeasureMM();
            const kn::engraving::Segment* segment = measure ? measure->first(kn::engraving::SegmentType::ChordRest) : nullptr;
            if (EngravingItem* item = segment ? segment->element(0) : nullptr) {
                interaction()->select({ item }, SelectType::SINGLE, 0);
            }
        }
    }

    const INotationNoteInputPtr noteInput = interaction()->noteInput();
    updatePadModels(noteInput->state().drumset());

    noteInput->stateChanged().onNotify(this, [this, updatePadModels]() {
        if (!notation()) {
            updatePadModels(nullptr);
            return;
        }
        const INotationNoteInputPtr ni = interaction()->noteInput();
        updatePadModels(ni->state().drumset());
    }, Asyncable::Mode::SetReplace /* FIXME */);

    if (audioSettings()) {
        audioSettings()->trackInputParamsChanged().onReceive(this, [this](InstrumentTrackId trackId) {
            if (trackId != currentTrackId()) {
                return;
            }
            updateSoundTitle(trackId);
        }, Asyncable::Mode::SetReplace /* FIXME */);
    }
}

void PercussionPanelModel::setDrumset(engraving::Drumset* drumset)
{
    m_padListModel->setDrumset(drumset);

    // If drumset contained drums with undefined values for panelRow/panelColumn, m_padListModel will have
    // assigned them now (see PercussionPanelPadListModel::load). In this case, we should update the score's
    // drumset so that it matches the one in the panel...

    const std::pair<Instrument*, Part*> instAndPart = getCurrentInstrumentAndPart();
    Instrument* inst = instAndPart.first;

    const Drumset* instDrumset = inst ? inst->drumset() : nullptr;
    const Drumset* panelDrumset = m_padListModel->drumset();

    if (instDrumset && panelDrumset && *instDrumset != *panelDrumset) {
        inst->setDrumset(panelDrumset);
    }
}

void PercussionPanelModel::updateSoundTitle(const InstrumentTrackId& trackId)
{
    if (!trackId.isValid()) {
        setSoundTitle(QString());
        return;
    }

    const audio::AudioInputParams& params = audioSettings()->trackInputParams(trackId);
    const QString name = kfw::audio::audioSourceName(params).toQString();
    if (name.isEmpty()) {
        setSoundTitle(QString());
        return;
    }

    const QString pack = kfw::audio::audioSourcePackName(params).toQString();
    if (!pack.isEmpty() && pack != name) {
        setSoundTitle(pack + ": " + name);
        return;
    }

    const QString category = kfw::audio::audioSourceCategoryName(params).toQString();
    if (!category.isEmpty() && category != name) {
        setSoundTitle(category + ": " + name);
        return;
    }

    setSoundTitle(name);
}

bool PercussionPanelModel::eventFilter(QObject* watched, QEvent* event)
{
    // Finish editing on escape...
    if (m_currentPanelMode != PanelMode::Mode::EDIT_LAYOUT || event->type() != QEvent::Type::ShortcutOverride
        || m_padListModel->swapInProgress()) {
        return QObject::eventFilter(watched, event);
    }
    QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
    if (!keyEvent || keyEvent->key() != Qt::Key_Escape) {
        return QObject::eventFilter(watched, event);
    }
    finishEditing();
    event->setAccepted(true);
    return true;
}

void PercussionPanelModel::onPadTriggered(int pitch, const PercussionPanelPadModel::PadAction& action)
{
    switch (currentPanelMode()) {
    case PanelMode::Mode::WRITE:
        writePitch(pitch, WRITE_ACTION_MAP.at(action));
        break;
    case PanelMode::Mode::SOUND_PREVIEW:
        playPitch(pitch);
        break;
    default: break;
    }
}

void PercussionPanelModel::onDuplicatePadRequested(int pitch)
{
    const std::pair<Instrument*, Part*> instAndPart = getCurrentInstrumentAndPart();
    Instrument* inst = instAndPart.first;
    Part* part = instAndPart.second;

    IF_ASSERT_FAILED(inst && part) {
        return;
    }

    const int nextAvailablePitch = m_padListModel->nextAvailablePitch(pitch);
    if (nextAvailablePitch < 0) {
        // TODO: Show some sort of warning dialog?
        LOGE() << "No space to duplicate drum pad";
        return;
    }
    const int nextAvailableIndex = m_padListModel->nextAvailableIndex(pitch);

    Drumset updatedDrumset = *m_padListModel->drumset();

    engraving::DrumInstrument duplicateDrum = updatedDrumset.drum(pitch);

    duplicateDrum.shortcut.clear(); // Don't steal the shortcut
    duplicateDrum.panelRow = nextAvailableIndex / m_padListModel->numColumns();
    duplicateDrum.panelColumn = nextAvailableIndex % m_padListModel->numColumns();

    updatedDrumset.setDrum(nextAvailablePitch, duplicateDrum);

    const InstrumentKey key = { inst->id(), part->id() };
    notation()->parts()->replaceDrumset(key, updatedDrumset);
}

void PercussionPanelModel::onDeletePadRequested(int pitch)
{
    const std::pair<Instrument*, Part*> instAndPart = getCurrentInstrumentAndPart();
    Instrument* inst = instAndPart.first;
    Part* part = instAndPart.second;

    IF_ASSERT_FAILED(inst && part) {
        return;
    }

    Drumset updatedDrumset = *m_padListModel->drumset();
    updatedDrumset.setDrum(pitch, engraving::DrumInstrument());

    const InstrumentKey key = { inst->id(), part->id() };
    notation()->parts()->replaceDrumset(key, updatedDrumset);
}

void PercussionPanelModel::onDefinePadShortcutRequested(int pitch)
{
    const std::pair<Instrument*, Part*> instAndPart = getCurrentInstrumentAndPart();
    Instrument* inst = instAndPart.first;
    Part* part = instAndPart.second;
    IF_ASSERT_FAILED(inst && part) {
        return;
    }

    Drumset updatedDrumset = *m_padListModel->drumset();
    if (!PercussionUtilities(iocContext()).editPercussionShortcut(updatedDrumset, pitch)) {
        return;
    }

    const InstrumentKey key = { inst->id(), part->id() };
    notation()->parts()->replaceDrumset(key, updatedDrumset);
}

void PercussionPanelModel::writePitch(int pitch, const NoteAddingMode& addingMode)
{
    INotationUndoStackPtr undoStack = notation()->undoStack();
    if (!interaction() || !undoStack) {
        return;
    }

    interaction()->noteInput()->startNoteInput(notationConfiguration()->defaultNoteInputMethod(), /*focusNotation*/ false);

    NoteInputParams params;
    params.drumPitch = pitch;

    const ActionData args = ActionData::make_arg2<NoteInputParams, NoteAddingMode>(params, addingMode);
    dispatcher()->dispatch("note-action", args);
}

void PercussionPanelModel::playPitch(int pitch)
{
    if (!interaction()) {
        return;
    }

    const NoteInputState& inputState = interaction()->noteInput()->state();
    std::shared_ptr<Chord> chord = PercussionUtilities(iocContext()).getDrumNoteForPreview(m_padListModel->drumset(), pitch);

    chord->setParent(inputState.segment());
    chord->setTrack(inputState.track());

    playbackController()->playElements({ chord.get() });
}

void PercussionPanelModel::resetLayout()
{
    if (m_currentPanelMode == PanelMode::Mode::EDIT_LAYOUT) {
        finishEditing(/*discardChanges*/ true);
    }

    const std::pair<Instrument*, Part*> instAndPart = getCurrentInstrumentAndPart();
    Instrument* inst = instAndPart.first;
    Part* part = instAndPart.second;

    IF_ASSERT_FAILED(audioSettings() && inst && part) {
        return;
    }

    Drumset defaultDrumset = standardDefaultDrumset();

    Drumset defaultLayout = m_padListModel->constructDefaultLayout(defaultDrumset);
    if (defaultLayout == *m_padListModel->drumset()) {
        // Nothing changed after reset...
        return;
    }

    const InstrumentKey key = { inst->id(), part->id() };
    notation()->parts()->replaceDrumset(key, defaultLayout);
}

Drumset PercussionPanelModel::standardDefaultDrumset() const
{
    const std::pair<Instrument*, Part*> instAndPart = getCurrentInstrumentAndPart();
    const Instrument* inst = instAndPart.first;
    const Part* part = instAndPart.second;

    IF_ASSERT_FAILED(inst && inst->drumset() && part) {
        return Drumset();
    }

    const InstrumentTemplate& instTemplate = instrumentsRepository()->instrumentTemplate(inst->id());
    IF_ASSERT_FAILED(instTemplate.drumset) {
        return Drumset();
    }

    return *instTemplate.drumset;
}

void PercussionPanelModel::setColumns(int numColumns)
{
    IF_ASSERT_FAILED(numColumns > 0) {
        return;
    }

    if (numColumns == m_padListModel->numColumns()) {
        return;
    }

    if (m_currentPanelMode == PanelMode::Mode::EDIT_LAYOUT) {
        finishEditing(/*discardChanges*/ true);
    }

    const std::pair<Instrument*, Part*> instAndPart = getCurrentInstrumentAndPart();
    Instrument* inst = instAndPart.first;
    Part* part = instAndPart.second;
    IF_ASSERT_FAILED(inst && inst->drumset() && part) {
        return;
    }

    Drumset updatedDrumset = *m_padListModel->drumset();

    // Calculate new values for panelRow & panelColumn...
    for (int pitch = 0; pitch < kn::engraving::DRUM_INSTRUMENTS; ++pitch) {
        if (!updatedDrumset.isValid(pitch)) {
            continue;
        }
        kn::engraving::DrumInstrument& drum = updatedDrumset.drum(pitch);
        const int index = drum.panelRow * static_cast<int>(updatedDrumset.percussionPanelColumns()) + drum.panelColumn;
        drum.panelRow = index / numColumns;
        drum.panelColumn = index % numColumns;
    }

    updatedDrumset.setPercussionPanelColumns(numColumns);

    const InstrumentKey key = { inst->id(), part->id() };
    notation()->parts()->replaceDrumset(key, updatedDrumset);
}

InstrumentTrackId PercussionPanelModel::currentTrackId() const
{
    if (!interaction()) {
        return InstrumentTrackId();
    }

    const NoteInputState& inputState = interaction()->noteInput()->state();
    const Staff* staff = inputState.staff();

    if (!staff || !staff->part() || !inputState.segment()) {
        return InstrumentTrackId();
    }

    return { staff->part()->id(), staff->part()->instrumentId(inputState.segment()->tick()) };
}

std::pair<kn::engraving::Instrument*, kn::engraving::Part*> PercussionPanelModel::getCurrentInstrumentAndPart() const
{
    if (!interaction()) {
        return { nullptr, nullptr };
    }

    const NoteInputState& inputState = interaction()->noteInput()->state();

    const Staff* staff = inputState.staff();

    Part* part = staff ? staff->part() : nullptr;

    if (!inputState.segment()) {
        return { nullptr, part };
    }

    Instrument* inst = part ? part->instrument(inputState.segment()->tick()) : nullptr;

    return { inst, part };
}

const kn::project::IProjectAudioSettingsPtr PercussionPanelModel::audioSettings() const
{
    return globalContext()->currentProject() ? globalContext()->currentProject()->audioSettings() : nullptr;
}

const INotationPtr PercussionPanelModel::notation() const
{
    return globalContext()->currentNotation();
}

const INotationInteractionPtr PercussionPanelModel::interaction() const
{
    return notation() ? notation()->interaction() : nullptr;
}

kn::engraving::Score* PercussionPanelModel::score() const
{
    return notation() ? notation()->elements()->msScore() : nullptr;
}
