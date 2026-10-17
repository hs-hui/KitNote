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
#include "noteheadsettingsmodel.h"

#include "engraving/types/types.h"

#include "translation.h"

using namespace kn::inspector;
using namespace kn::engraving;

NoteheadSettingsModel::NoteheadSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                             IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setTitle(kfw::qtrc("inspector", "Head"));
    setModelType(InspectorModelType::TYPE_NOTEHEAD);

    createProperties();
}

void NoteheadSettingsModel::createProperties()
{
    m_isHeadHidden = buildPropertyItem(kn::engraving::Pid::VISIBLE, [this](const kn::engraving::Pid pid, const QVariant& isHeadHidden) {
        onPropertyValueChanged(pid, !isHeadHidden.toBool());
    });

    m_isHeadSmall = buildPropertyItem(kn::engraving::Pid::SMALL);
    m_hasHeadParentheses = buildPropertyItem(kn::engraving::Pid::HAS_PARENTHESES, [&](const kn::engraving::Pid, const QVariant& hasParens) {
        if (m_elementList.empty()) {
            return;
        }
        Score* score = m_elementList.front()->score();
        if (hasParens.toBool()) {
            beginCommand(TranslatableString("undoableAction", "Add notehead parentheses"));
            score->cmdAddParenthesesToNotes();
        } else {
            beginCommand(TranslatableString("undoableAction", "Remove notehead parentheses"));
            score->cmdRemoveParenthesesFromNotes();
        }
        updateNotation();
        endCommand();
        loadProperties();
    }, [this](const kn::engraving::Sid styleId, const QVariant& newValue) {
        updateStyleValue(styleId, newValue);

        emit requestReloadPropertyItems();
    }, [this](const kn::engraving::Pid propertyId) {
        if (m_elementList.empty()) {
            return;
        }

        beginCommand(TranslatableString("undoableAction", "Reset %1").arg(propertyUserName(propertyId)));

        for (kn::engraving::EngravingItem* item : m_elementList) {
            IF_ASSERT_FAILED(item) {
                continue;
            }

            item->undoResetProperty(propertyId);
        }
        Score* score = m_elementList.front()->score();
        bool addParens = m_elementList.front()->parenthesesMode() == ParenthesesMode::BOTH;
        if (addParens) {
            score->cmdAddParenthesesToNotes();
        } else {
            score->cmdRemoveParenthesesFromNotes();
        }
        updateNotation();
        endCommand();
        loadProperties();
    });
    m_headDirection = buildPropertyItem(kn::engraving::Pid::MIRROR_HEAD);
    m_headGroup = buildPropertyItem(kn::engraving::Pid::HEAD_GROUP);
    m_headType = buildPropertyItem(kn::engraving::Pid::HEAD_TYPE);
    m_headSystem = buildPropertyItem(kn::engraving::Pid::HEAD_SCHEME);
    m_dotPosition = buildPropertyItem(kn::engraving::Pid::DOT_POSITION);
    m_offset = buildPointFPropertyItem(kn::engraving::Pid::OFFSET);
}

void NoteheadSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::NOTEHEAD);
}

void NoteheadSettingsModel::loadProperties()
{
    static PropertyIdSet propertyIdSet {
        Pid::VISIBLE,
        Pid::SMALL,
        Pid::HAS_PARENTHESES,
        Pid::MIRROR_HEAD,
        Pid::HEAD_GROUP,
        Pid::HEAD_TYPE,
        Pid::HEAD_SCHEME,
        Pid::DOT_POSITION,
        Pid::OFFSET,
    };

    loadProperties(propertyIdSet);
    updateIsTrillCueNote();
    updatemeasurementUnits();
}

void NoteheadSettingsModel::resetProperties()
{
    m_isHeadHidden->resetToDefault();
    m_isHeadSmall->resetToDefault();
    m_hasHeadParentheses->resetToDefault();
    m_headDirection->resetToDefault();
    m_headGroup->resetToDefault();
    m_headType->resetToDefault();
    m_headSystem->resetToDefault();
    m_dotPosition->resetToDefault();
    m_offset->resetToDefault();
}

void NoteheadSettingsModel::onNotationChanged(const kn::engraving::PropertyIdSet& changedPropertyIdSet, const kn::engraving::StyleIdSet&)
{
    loadProperties(changedPropertyIdSet);
}

void NoteheadSettingsModel::loadProperties(const kn::engraving::PropertyIdSet& propertyIdSet)
{
    if (kfw::contains(propertyIdSet, Pid::VISIBLE)) {
        loadPropertyItem(m_isHeadHidden, [](const QVariant& isVisible) -> QVariant {
            return !isVisible.toBool();
        });
    }

    if (kfw::contains(propertyIdSet, Pid::SMALL)) {
        loadPropertyItem(m_isHeadSmall);
    }

    if (kfw::contains(propertyIdSet, Pid::HAS_PARENTHESES)) {
        loadPropertyItem(m_hasHeadParentheses);
    }

    if (kfw::contains(propertyIdSet, Pid::MIRROR_HEAD)) {
        loadPropertyItem(m_headDirection);
    }

    if (kfw::contains(propertyIdSet, Pid::HEAD_GROUP)) {
        loadPropertyItem(m_headGroup);
    }

    if (kfw::contains(propertyIdSet, Pid::HEAD_TYPE)) {
        loadPropertyItem(m_headType);
    }

    if (kfw::contains(propertyIdSet, Pid::HEAD_SCHEME)) {
        loadPropertyItem(m_headSystem);
    }

    if (kfw::contains(propertyIdSet, Pid::DOT_POSITION)) {
        loadPropertyItem(m_dotPosition);
    }

    if (kfw::contains(propertyIdSet, Pid::OFFSET)) {
        loadPropertyItem(m_offset);
    }
}

PropertyItem* NoteheadSettingsModel::isHeadHidden() const
{
    return m_isHeadHidden;
}

PropertyItem* NoteheadSettingsModel::isHeadSmall() const
{
    return m_isHeadSmall;
}

PropertyItem* NoteheadSettingsModel::hasHeadParentheses() const
{
    return m_hasHeadParentheses;
}

PropertyItem* NoteheadSettingsModel::headDirection() const
{
    return m_headDirection;
}

PropertyItem* NoteheadSettingsModel::headGroup() const
{
    return m_headGroup;
}

PropertyItem* NoteheadSettingsModel::headType() const
{
    return m_headType;
}

PropertyItem* NoteheadSettingsModel::headSystem() const
{
    return m_headSystem;
}

PropertyItem* NoteheadSettingsModel::dotPosition() const
{
    return m_dotPosition;
}

PropertyItem* NoteheadSettingsModel::offset() const
{
    return m_offset;
}

bool NoteheadSettingsModel::isTrillCueNote() const
{
    return m_isTrillCueNote;
}

QVariantList NoteheadSettingsModel::possibleHeadSystemTypes() const
{
    QMap<kn::engraving::NoteHeadScheme, QString> types {
        { kn::engraving::NoteHeadScheme::HEAD_AUTO,                    kfw::qtrc("inspector", "Auto", "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_NORMAL,                  kfw::qtrc("inspector", "Normal", "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_PITCHNAME,               kfw::qtrc("inspector", "Pitch names", "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_PITCHNAME_NO_ACCIDENTALS, kfw::qtrc("inspector", "Pitch names, no accidentals",
                                                                                   "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_PITCHNAME_GERMAN,        kfw::qtrc("inspector", "German pitch names", "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_PITCHNAME_GERMAN_NO_ACCIDENTALS, kfw::qtrc("inspector", "German pitch names, no accidentals",
                                                                                          "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_SOLFEGE,                 kfw::qtrc("inspector", "Solfège movable do", "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_SOLFEGE_FIXED,           kfw::qtrc("inspector", "Solfège fixed do", "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_SHAPE_NOTE_4,            kfw::qtrc("inspector", "4-shape (Walker)", "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_SHAPE_NOTE_7_AIKIN,      kfw::qtrc("inspector", "7-shape (Aikin)", "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_SHAPE_NOTE_7_FUNK,       kfw::qtrc("inspector", "7-shape (Funk)", "notehead scheme") },
        { kn::engraving::NoteHeadScheme::HEAD_SHAPE_NOTE_7_WALKER,     kfw::qtrc("inspector", "7-shape (Walker)", "notehead scheme") },
    };

    QVariantList result;

    for (kn::engraving::NoteHeadScheme type : types.keys()) {
        QVariantMap obj;

        obj["text"] = types[type];
        obj["value"] = static_cast<int>(type);

        result << obj;
    }

    return result;
}

void NoteheadSettingsModel::updateIsTrillCueNote()
{
    bool isTrillCueNote = true;
    for (EngravingItem* item : m_elementList) {
        if (item->isNote() && !toNote(item)->isTrillCueNote()) {
            isTrillCueNote = false;
            break;
        }
    }
    setIsTrillCueNote(isTrillCueNote);
}

void NoteheadSettingsModel::setIsTrillCueNote(bool v)
{
    if (v == m_isTrillCueNote) {
        return;
    }

    m_isTrillCueNote = v;
    emit isTrillCueNoteChanged(m_isTrillCueNote);
}
