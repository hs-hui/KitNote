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
#include "chordsettingsmodel.h"

#include "translation.h"

using namespace kn::inspector;
using namespace kn::engraving;

ChordSettingsModel::ChordSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_CHORD);
    setTitle(kfw::qtrc("inspector", "Chord"));

    createProperties();
}

void ChordSettingsModel::createProperties()
{
    m_isStemless = buildPropertyItem(kn::engraving::Pid::NO_STEM,
                                     [this](const kn::engraving::Pid pid, const QVariant& newValue) {
        onPropertyValueChanged(pid, newValue);
        updateShowStemSlashEnabled();
    });

    m_combineVoice = buildPropertyItem(Pid::COMBINE_VOICE);
    m_showStemSlash = buildPropertyItem(Pid::SHOW_STEM_SLASH);
}

void ChordSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(ElementType::CHORD);
}

void ChordSettingsModel::loadProperties()
{
    loadPropertyItem(m_isStemless);
    loadPropertyItem(m_showStemSlash);
    loadPropertyItem(m_combineVoice);
    updateShowStemSlashVisible();
    updateShowStemSlashEnabled();
}

void ChordSettingsModel::resetProperties()
{
    m_isStemless->resetToDefault();
    m_showStemSlash->resetToDefault();
    updateShowStemSlashEnabled();
}

PropertyItem* ChordSettingsModel::isStemless() const
{
    return m_isStemless;
}

PropertyItem* ChordSettingsModel::showStemSlash() const
{
    return m_showStemSlash;
}

PropertyItem* ChordSettingsModel::combineVoice() const
{
    return m_combineVoice;
}

bool ChordSettingsModel::showStemSlashVisible() const
{
    return m_showStemSlashVisible;
}

bool ChordSettingsModel::showStemSlashEnabled() const
{
    return m_showStemSlashEnabled;
}

void ChordSettingsModel::updateShowStemSlashVisible()
{
    bool visible = false;
    for (EngravingItem* element : m_elementList) {
        engraving::EngravingItem* elementBase = element->elementBase();
        if (elementBase->isChord()) {
            engraving::Chord* chord = engraving::toChord(elementBase);
            if (chord->noteType() != NoteType::NORMAL) {
                visible = true;
                break;
            }
        }
    }
    setShowStemSlashVisible(visible);
}

void ChordSettingsModel::updateShowStemSlashEnabled()
{
    bool enabled = false;
    for (EngravingItem* element : m_elementList) {
        engraving::EngravingItem* elementBase = element->elementBase();
        if (elementBase->isChord()) {
            engraving::Chord* chord = engraving::toChord(elementBase);
            if (!chord->noStem()) {
                enabled = true;
                break;
            }
        }
    }
    setShowStemSlashEnabled(enabled);
}

void ChordSettingsModel::setShowStemSlashVisible(bool visible)
{
    if (visible == m_showStemSlashVisible) {
        return;
    }

    m_showStemSlashVisible = visible;
    emit showStemSlashVisibleChanged(m_showStemSlashVisible);
}

void ChordSettingsModel::setShowStemSlashEnabled(bool enabled)
{
    if (enabled == m_showStemSlashEnabled) {
        return;
    }

    m_showStemSlashEnabled = enabled;
    emit showStemSlashEnabledChanged(m_showStemSlashEnabled);
}
