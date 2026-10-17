/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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

#include "abstractelementpopupmodel.h"

#include "engraving/dom/property.h"

#include "elementpopups/partialtiepopupmodel.h"
#include "elementpopups/shadownotepopupmodel.h"

#include "log.h"

using namespace kn::notation;

static const QMap<kn::engraving::ElementType, PopupModelType> ELEMENT_POPUP_TYPES = {
    { kn::engraving::ElementType::HARP_DIAGRAM, PopupModelType::TYPE_HARP_DIAGRAM },
    { kn::engraving::ElementType::CAPO, PopupModelType::TYPE_CAPO },
    { kn::engraving::ElementType::STRING_TUNINGS, PopupModelType::TYPE_STRING_TUNINGS },
    { kn::engraving::ElementType::SOUND_FLAG, PopupModelType::TYPE_SOUND_FLAG },
    { kn::engraving::ElementType::STAFF_VISIBILITY_INDICATOR, PopupModelType::TYPE_STAFF_VISIBILITY },
    { kn::engraving::ElementType::DYNAMIC, PopupModelType::TYPE_DYNAMIC },
    { kn::engraving::ElementType::TEXT, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::STAFF_TEXT, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::SYSTEM_TEXT, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::EXPRESSION, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::PLAYTECH_ANNOTATION, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::REHEARSAL_MARK, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::INSTRUMENT_CHANGE, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::FINGERING, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::STICKING, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::LYRICS, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::FIGURED_BASS, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::TEMPO_TEXT, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::PLAY_COUNT_TEXT, PopupModelType::TYPE_TEXT },
    { kn::engraving::ElementType::TIE_SEGMENT, PopupModelType::TYPE_PARTIAL_TIE },
    { kn::engraving::ElementType::PARTIAL_TIE_SEGMENT, PopupModelType::TYPE_PARTIAL_TIE },
    { kn::engraving::ElementType::SHADOW_NOTE, PopupModelType::TYPE_SHADOW_NOTE },
};

static const QHash<PopupModelType, kn::engraving::ElementTypeSet> POPUP_DEPENDENT_ELEMENT_TYPES = {
    { PopupModelType::TYPE_HARP_DIAGRAM, { kn::engraving::ElementType::HARP_DIAGRAM } },
    { PopupModelType::TYPE_CAPO, { kn::engraving::ElementType::CAPO } },
    { PopupModelType::TYPE_STRING_TUNINGS, { kn::engraving::ElementType::STRING_TUNINGS } },
    { PopupModelType::TYPE_SOUND_FLAG, { kn::engraving::ElementType::SOUND_FLAG, kn::engraving::ElementType::STAFF_TEXT } },
    { PopupModelType::TYPE_STAFF_VISIBILITY, { kn::engraving::ElementType::STAFF_VISIBILITY_INDICATOR } },
    { PopupModelType::TYPE_DYNAMIC, { kn::engraving::ElementType::DYNAMIC } },
    { PopupModelType::TYPE_TEXT,
      { kn::engraving::ElementType::TEXT,
        kn::engraving::ElementType::STAFF_TEXT,
        kn::engraving::ElementType::SYSTEM_TEXT,
        kn::engraving::ElementType::EXPRESSION,
        kn::engraving::ElementType::PLAYTECH_ANNOTATION,
        kn::engraving::ElementType::REHEARSAL_MARK,
        kn::engraving::ElementType::INSTRUMENT_CHANGE,
        kn::engraving::ElementType::FINGERING,
        kn::engraving::ElementType::STICKING,
        kn::engraving::ElementType::LYRICS,
        kn::engraving::ElementType::FIGURED_BASS,
        kn::engraving::ElementType::TEMPO_TEXT,
        kn::engraving::ElementType::PLAY_COUNT_TEXT } },
    { PopupModelType::TYPE_PARTIAL_TIE, { kn::engraving::ElementType::PARTIAL_TIE_SEGMENT, kn::engraving::ElementType::TIE_SEGMENT } },
};

AbstractElementPopupModel::AbstractElementPopupModel(PopupModelType modelType, QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this)), m_modelType(modelType)
{
}

PopupModelType AbstractElementPopupModel::modelType() const
{
    return m_modelType;
}

QRect AbstractElementPopupModel::itemRect() const
{
    return m_itemRect;
}

bool AbstractElementPopupModel::hasElementEditPopup(const EngravingItem* element)
{
    if (!element) {
        return false;
    }

    const PopupModelType modelType = modelTypeFromElement(element->type());
    if (modelType == PopupModelType::TYPE_UNDEFINED) {
        return false;
    }

    if (modelType == PopupModelType::TYPE_TEXT) {
        // Text style popup is only opened when making a selection during text editing
        return false;
    }

    switch (modelType) {
    case PopupModelType::TYPE_PARTIAL_TIE:
        return PartialTiePopupModel::canOpen(element);
    case PopupModelType::TYPE_SHADOW_NOTE:
        return ShadowNotePopupModel::canOpen(element);
    default:
        return true;
    }
}

bool AbstractElementPopupModel::hasTextStylePopup(const EngravingItem* element)
{
    if (!element) {
        return false;
    }

    const PopupModelType modelType = modelTypeFromElement(element->type());
    return modelType == PopupModelType::TYPE_TEXT;
}

PopupModelType AbstractElementPopupModel::modelTypeFromElement(const engraving::ElementType& elementType)
{
    return ELEMENT_POPUP_TYPES.value(elementType, PopupModelType::TYPE_UNDEFINED);
}

kfw::PointF AbstractElementPopupModel::fromLogical(kfw::PointF point) const
{
    return currentNotation()->viewState() ? currentNotation()->viewState()->matrix().map(point) : kfw::PointF();
}

kfw::RectF AbstractElementPopupModel::fromLogical(kfw::RectF rect) const
{
    return currentNotation()->viewState() ? currentNotation()->viewState()->matrix().map(rect) : kfw::RectF();
}

INotationUndoStackPtr AbstractElementPopupModel::undoStack() const
{
    return currentNotation() ? currentNotation()->undoStack() : nullptr;
}

void AbstractElementPopupModel::beginCommand(const kfw::TranslatableString& actionName)
{
    if (undoStack()) {
        undoStack()->prepareChanges(actionName);
    }
}

void AbstractElementPopupModel::beginMultiCommands(const kfw::TranslatableString& actionName)
{
    beginCommand(actionName);

    if (undoStack()) {
        undoStack()->lock();
    }
}

void AbstractElementPopupModel::endCommand()
{
    if (undoStack()) {
        undoStack()->commitChanges();
    }
}

void AbstractElementPopupModel::endMultiCommands()
{
    if (undoStack()) {
        undoStack()->unlock();
    }

    endCommand();
}

void AbstractElementPopupModel::updateNotation()
{
    if (!currentNotation()) {
        return;
    }

    currentNotation()->notationChanged().notify();
}

INotationPtr AbstractElementPopupModel::currentNotation() const
{
    return globalContext()->currentNotation();
}

void AbstractElementPopupModel::changeItemProperty(kn::engraving::Pid id, const PropertyValue& value)
{
    IF_ASSERT_FAILED(m_item) {
        return;
    }

    kn::engraving::PropertyFlags flags = m_item->propertyFlags(id);
    if (flags == kn::engraving::PropertyFlags::STYLED) {
        flags = kn::engraving::PropertyFlags::UNSTYLED;
    }

    beginCommand(kfw::TranslatableString("undoableAction", "Edit %1").arg(kn::engraving::propertyUserName(id)));
    m_item->undoChangeProperty(id, value, flags);
    endCommand();
    updateNotation();
}

void AbstractElementPopupModel::changeItemProperty(kn::engraving::Pid id, const PropertyValue& value, kn::engraving::PropertyFlags flags)
{
    IF_ASSERT_FAILED(m_item) {
        return;
    }

    beginCommand(kfw::TranslatableString("undoableAction", "Edit %1").arg(kn::engraving::propertyUserName(id)));
    m_item->undoChangeProperty(id, value, flags);
    endCommand();
    updateNotation();
}

INotationInteractionPtr AbstractElementPopupModel::interaction() const
{
    INotationPtr notation = globalContext()->currentNotation();
    return notation ? notation->interaction() : nullptr;
}

INotationSelectionPtr AbstractElementPopupModel::selection() const
{
    INotationInteractionPtr interaction = this->interaction();
    return interaction ? interaction->selection() : nullptr;
}

void AbstractElementPopupModel::init()
{
    m_item = nullptr;

    INotationSelectionPtr selection = this->selection();
    if (!selection) {
        return;
    }

    INotationUndoStackPtr undoStack = this->undoStack();
    if (!undoStack) {
        return;
    }

    m_item = selection->element();

    undoStack->changesChannel().onReceive(this, [this] (const ScoreChanges& changes) {
        if (ignoreTextEditingChanges() && changes.isTextEditing) {
            return;
        }

        for (ElementType type : dependentElementTypes()) {
            if (kfw::contains(changes.changedTypes, type)) {
                emit dataChanged();
                updateItemRect();
                return;
            }
        }
    });

    updateItemRect();
}

kn::engraving::ElementType AbstractElementPopupModel::elementType() const
{
    return ELEMENT_POPUP_TYPES.key(m_modelType, ElementType::INVALID);
}

const kn::engraving::ElementTypeSet& AbstractElementPopupModel::dependentElementTypes() const
{
    auto it = POPUP_DEPENDENT_ELEMENT_TYPES.find(m_modelType);
    if (it != POPUP_DEPENDENT_ELEMENT_TYPES.end()) {
        return it.value();
    }

    static const engraving::ElementTypeSet dummy;
    return dummy;
}

void AbstractElementPopupModel::updateItemRect()
{
    const QRect rect = m_item ? fromLogical(m_item->canvasBoundingRect()).toQRect() : QRect();

    if (m_itemRect != rect) {
        m_itemRect = rect;
        emit itemRectChanged(rect);
    }
}
