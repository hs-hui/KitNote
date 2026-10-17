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
#include "accidentalsettingsmodel.h"

#include "translation.h"

#include "engraving/dom/accidental.h"

using namespace kn::inspector;
using namespace kn::engraving;

AccidentalSettingsModel::AccidentalSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                                 IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_ACCIDENTAL);
    setTitle(kfw::qtrc("inspector", "Accidental"));
    setIcon(kfw::ui::IconCode::Code::ACCIDENTAL_SHARP);
    createProperties();
}

void AccidentalSettingsModel::createProperties()
{
    m_bracketType = buildPropertyItem(kn::engraving::Pid::ACCIDENTAL_BRACKET);
    m_isSmall = buildPropertyItem(kn::engraving::Pid::SMALL);
    m_stackingOrderOffset = buildPropertyItem(kn::engraving::Pid::ACCIDENTAL_STACKING_ORDER_OFFSET,
                                              [this](const kn::engraving::Pid pid, const QVariant& newValue) {
        onPropertyValueChanged(pid, newValue);
        loadPropertyItem(m_stackingOrderOffset);
    });
}

void AccidentalSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::ACCIDENTAL);
}

void AccidentalSettingsModel::loadProperties()
{
    loadPropertyItem(m_bracketType);
    loadPropertyItem(m_isSmall);
    updateIsSmallAvailable();
    loadPropertyItem(m_stackingOrderOffset);
    updateIsStackingOrderAvailableAndEnabled();
}

void AccidentalSettingsModel::resetProperties()
{
    m_bracketType->resetToDefault();
    m_stackingOrderOffset->resetToDefault();
}

PropertyItem* AccidentalSettingsModel::bracketType() const
{
    return m_bracketType;
}

PropertyItem* AccidentalSettingsModel::isSmall() const
{
    return m_isSmall;
}

bool AccidentalSettingsModel::isSmallAvailable() const
{
    return m_isSmallAvailable;
}

PropertyItem* AccidentalSettingsModel::stackingOrderOffset() const
{
    return m_stackingOrderOffset;
}

bool AccidentalSettingsModel::isStackingOrderAvailable() const
{
    return m_isStackinOrderAvailable;
}

bool AccidentalSettingsModel::isStackingOrderEnabled() const
{
    return m_isStackingOrderEnabled;
}

void AccidentalSettingsModel::updateIsSmallAvailable()
{
    bool available = true;
    for (kn::engraving::EngravingItem* item : m_elementList) {
        kn::engraving::EngravingItem* parent = item ? item->parentItem() : nullptr;
        if (parent && (parent->isOrnament() || parent->isTrillSegment())) {
            available = false;
            break;
        }
    }
    setIsSmallAvailable(available);
}

void AccidentalSettingsModel::setIsSmallAvailable(bool available)
{
    if (m_isSmallAvailable == available) {
        return;
    }

    m_isSmallAvailable = available;
    emit isSmallAvailableChanged(m_isSmallAvailable);
}

void AccidentalSettingsModel::updateIsStackingOrderAvailableAndEnabled()
{
    setIsStackingOrderEnabled(m_elementList.size() == 1);

    for (EngravingItem* item : m_elementList) {
        if (!item->isAccidental() || !toAccidental(item)->note()) {
            continue;
        }
        setIsStackingOrderEnabled(item->addToSkyline());

        Segment* segment = toAccidental(item)->note()->chord()->segment();
        track_idx_t startTrack = trackZeroVoice(item->track());
        track_idx_t endTrack = startTrack + VOICES;
        for (track_idx_t track = startTrack; track < endTrack; ++track) {
            EngravingItem* elem = segment->element(track);
            if (!elem || !elem->isChord()) {
                continue;
            }
            for (Note* note : toChord(elem)->notes()) {
                if (note->accidental() && note->accidental()->ldata()->column > 0) {
                    setIsStackingOrderAvailable(true);
                    return;
                }
            }
        }
    }

    setIsStackingOrderAvailable(false);
}

void AccidentalSettingsModel::setIsStackingOrderAvailable(bool available)
{
    if (m_isStackinOrderAvailable == available) {
        return;
    }

    m_isStackinOrderAvailable = available;
    emit isStackingOrderAvailableChanged(m_isStackinOrderAvailable);
}

void AccidentalSettingsModel::setIsStackingOrderEnabled(bool enabled)
{
    if (m_isStackingOrderEnabled == enabled) {
        return;
    }

    m_isStackingOrderEnabled = enabled;
    emit isStackingOrderEnabledChanged(m_isStackingOrderEnabled);
}
