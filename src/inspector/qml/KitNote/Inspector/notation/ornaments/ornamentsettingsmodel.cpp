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
#include "ornamentsettingsmodel.h"
#include "types/ornamenttypes.h"

#include "engraving/dom/articulation.h"
#include "engraving/dom/ornament.h"

#include "log.h"
#include "translation.h"

using namespace kn::inspector;
using kn::engraving::Pid;
using kn::engraving::PropertyValue;
using kn::engraving::OrnamentInterval;
using kn::engraving::IntervalStep;
using kn::engraving::IntervalType;
using kn::engraving::P_TYPE;
using kn::engraving::Ornament;

OrnamentSettingsModel::OrnamentSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                             IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_ORNAMENT);
    setTitle(kfw::qtrc("inspector", "Ornament"));
    setIcon(kfw::ui::IconCode::Code::ORNAMENT);
    createProperties();
}

void OrnamentSettingsModel::createProperties()
{
    auto onIntervalChanged = [this](const Pid pid, const QVariant& newValue) {
        OrnamentTypes::BasicInterval basicInterval = OrnamentTypes::BasicInterval(newValue.toInt());
        OrnamentInterval newInterval;

        switch (basicInterval) {
        case OrnamentTypes::BasicInterval::TYPE_AUTO_DIATONIC:
            newInterval.step = IntervalStep::SECOND;
            newInterval.type = IntervalType::AUTO;
            break;
        case OrnamentTypes::BasicInterval::TYPE_MAJOR_SECOND:
            newInterval.step = IntervalStep::SECOND;
            newInterval.type = IntervalType::MAJOR;
            break;
        case OrnamentTypes::BasicInterval::TYPE_MINOR_SECOND:
            newInterval.step = IntervalStep::SECOND;
            newInterval.type = IntervalType::MINOR;
            break;
        case OrnamentTypes::BasicInterval::TYPE_AUGMENTED_SECOND:
            newInterval.step = IntervalStep::SECOND;
            newInterval.type = IntervalType::AUGMENTED;
            break;
        default:
            break;
        }

        onPropertyValueChanged(pid, PropertyValue(newInterval).toQVariant());
    };

    m_placement = buildPropertyItem(Pid::ARTICULATION_ANCHOR);

    m_intervalAbove = buildPropertyItem(Pid::INTERVAL_ABOVE, onIntervalChanged);
    m_intervalBelow = buildPropertyItem(Pid::INTERVAL_BELOW, onIntervalChanged);

    m_intervalStep = buildPropertyItem(Pid::INTERVAL_ABOVE, [this](Pid id, const QVariant& newValue) {
        IntervalStep step = IntervalStep(newValue.toInt());
        setIntervalStep(id, step);
    });

    m_intervalType = buildPropertyItem(Pid::INTERVAL_ABOVE, [this](Pid id, const QVariant& newValue) {
        IntervalType type = IntervalType(newValue.toInt());
        setIntervalType(id, type);
    });

    m_showAccidental = buildPropertyItem(Pid::ORNAMENT_SHOW_ACCIDENTAL);
    m_showCueNote = buildPropertyItem(Pid::ORNAMENT_SHOW_CUE_NOTE);
    m_startOnUpperNote = buildPropertyItem(Pid::START_ON_UPPER_NOTE);
}

void OrnamentSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::ORNAMENT);
}

void OrnamentSettingsModel::loadProperties()
{
    auto convertIntevalValue = [](const QVariant& elementPropertyValue) -> QVariant {
        PropertyValue propertyValue = PropertyValue::fromQVariant(elementPropertyValue, P_TYPE::ORNAMENT_INTERVAL);
        OrnamentInterval interval = propertyValue.value<OrnamentInterval>();
        OrnamentTypes::BasicInterval basicInterval = OrnamentTypes::BasicInterval::TYPE_INVALID;

        if (interval.step == IntervalStep::SECOND && interval.type == IntervalType::AUTO) {
            basicInterval = OrnamentTypes::BasicInterval::TYPE_AUTO_DIATONIC;
        } else if (interval.step == IntervalStep::SECOND && interval.type == IntervalType::MAJOR) {
            basicInterval = OrnamentTypes::BasicInterval::TYPE_MAJOR_SECOND;
        } else if (interval.step == IntervalStep::SECOND && interval.type == IntervalType::MINOR) {
            basicInterval = OrnamentTypes::BasicInterval::TYPE_MINOR_SECOND;
        } else if (interval.step == IntervalStep::SECOND && interval.type == IntervalType::AUGMENTED) {
            basicInterval = OrnamentTypes::BasicInterval::TYPE_AUGMENTED_SECOND;
        }

        return static_cast<int>(basicInterval);
    };

    loadPropertyItem(m_placement);

    loadPropertyItem(m_intervalAbove, convertIntevalValue);
    loadPropertyItem(m_intervalBelow, convertIntevalValue);

    loadPropertyItem(m_intervalStep, [](const QVariant& elementPropertyValue) -> QVariant {
        PropertyValue propertyValue = PropertyValue::fromQVariant(elementPropertyValue, P_TYPE::ORNAMENT_INTERVAL);
        OrnamentInterval interval = propertyValue.value<OrnamentInterval>();
        return static_cast<int>(interval.step);
    });

    loadPropertyItem(m_intervalType, [](const QVariant& elementPropertyValue) -> QVariant {
        PropertyValue propertyValue = PropertyValue::fromQVariant(elementPropertyValue, P_TYPE::ORNAMENT_INTERVAL);
        OrnamentInterval interval = propertyValue.value<OrnamentInterval>();
        return static_cast<int>(interval.type);
    });

    loadPropertyItem(m_showAccidental);
    loadPropertyItem(m_showCueNote);
    loadPropertyItem(m_startOnUpperNote);

    updateIsFullIntervalChoiceAvailable();
    updateIsPerfectStep();
    updateIsIntervalAboveAvailable();
    updateIsIntervalBelowAvailable();
}

void OrnamentSettingsModel::resetProperties()
{
    m_placement->resetToDefault();
    m_intervalAbove->resetToDefault();
    m_intervalBelow->resetToDefault();
    m_intervalStep->resetToDefault();
    m_intervalType->resetToDefault();
    m_showAccidental->resetToDefault();
    m_showCueNote->resetToDefault();
    m_startOnUpperNote->resetToDefault();
}

PropertyItem* OrnamentSettingsModel::placement() const
{
    return m_placement;
}

PropertyItem* OrnamentSettingsModel::intervalAbove() const
{
    return m_intervalAbove;
}

PropertyItem* OrnamentSettingsModel::intervalBelow() const
{
    return m_intervalBelow;
}

PropertyItem* OrnamentSettingsModel::intervalStep() const
{
    return m_intervalStep;
}

PropertyItem* OrnamentSettingsModel::intervalType() const
{
    return m_intervalType;
}

PropertyItem* OrnamentSettingsModel::showAccidental() const
{
    return m_showAccidental;
}

PropertyItem* OrnamentSettingsModel::showCueNote() const
{
    return m_showCueNote;
}

PropertyItem* OrnamentSettingsModel::startOnUpperNote() const
{
    return m_startOnUpperNote;
}

bool OrnamentSettingsModel::isIntervalAboveAvailable() const
{
    return m_isIntervalAboveAvailable;
}

bool OrnamentSettingsModel::isIntervalBelowAvailable() const
{
    return m_isIntervalBelowAvailable;
}

bool OrnamentSettingsModel::isFullIntervalChoiceAvailable() const
{
    return m_isFullIntervalChoiceAvailable;
}

bool OrnamentSettingsModel::isPerfectStep() const
{
    return m_isPerfectStep;
}

void OrnamentSettingsModel::updateIsFullIntervalChoiceAvailable()
{
    bool available = true;
    for (kn::engraving::EngravingItem* item : m_elementList) {
        if (!item) {
            continue;
        }
        if (item->isOrnament()) {
            kn::engraving::Ornament* ornament = toOrnament(item);
            if (!ornament->hasFullIntervalChoice()) {
                available = false;
                break;
            }
        }
    }

    setIsFullIntervalChoiceAvailable(available);
}

void OrnamentSettingsModel::updateIsPerfectStep()
{
    int intervalStepValue = !m_intervalStep->isUndefined() ? m_intervalStep->value().toInt() : -1;
    IntervalStep intervalStep = static_cast<IntervalStep>(intervalStepValue);

    setIsPerfectStep(OrnamentInterval::isPerfectStep(intervalStep));
}

void OrnamentSettingsModel::setIntervalStep(Pid id, engraving::IntervalStep step)
{
    if (m_elementList.empty()) {
        return;
    }

    beginCommand(kfw::TranslatableString("undoableAction", "Set ornament interval step"));

    for (kn::engraving::EngravingItem* item : m_elementList) {
        IF_ASSERT_FAILED(item) {
            continue;
        }
        if (!item->isOrnament()) {
            continue;
        }

        OrnamentInterval interval = toOrnament(item)->intervalAbove();
        interval.step = step;
        if (interval.isPerfect() && (interval.type == IntervalType::MAJOR || interval.type == IntervalType::MINOR)) {
            interval.type = IntervalType::PERFECT;
        } else if (!interval.isPerfect() && interval.type == IntervalType::PERFECT) {
            interval.type = IntervalType::AUTO;
        }

        item->undoChangeProperty(id, interval);
    }

    updateNotation();
    loadProperties();
    endCommand();
}

void OrnamentSettingsModel::setIntervalType(Pid id, engraving::IntervalType type)
{
    if (m_elementList.empty()) {
        return;
    }

    beginCommand(kfw::TranslatableString("undoableAction", "Set ornament interval type"));

    for (kn::engraving::EngravingItem* item : m_elementList) {
        IF_ASSERT_FAILED(item) {
            continue;
        }
        if (!item->isOrnament()) {
            continue;
        }

        OrnamentInterval interval = static_cast<kn::engraving::Ornament*>(item)->intervalAbove();
        interval.type = type;
        item->undoChangeProperty(id, interval);
    }

    updateNotation();
    loadProperties();
    endCommand();
}

void OrnamentSettingsModel::updateIsIntervalAboveAvailable()
{
    if (m_isFullIntervalChoiceAvailable) {
        setIsIntervalAboveAvailable(false);
        return;
    }

    bool available = true;
    for (kn::engraving::EngravingItem* item : m_elementList) {
        if (!item) {
            continue;
        }
        if (item->isOrnament()) {
            kn::engraving::Ornament* ornament = toOrnament(item);
            if (!ornament->hasIntervalAbove()) {
                available = false;
                break;
            }
        }
    }

    setIsIntervalAboveAvailable(available);
}

void OrnamentSettingsModel::updateIsIntervalBelowAvailable()
{
    if (m_isFullIntervalChoiceAvailable) {
        setIsIntervalBelowAvailable(false);
        return;
    }

    bool available = true;
    for (kn::engraving::EngravingItem* item : m_elementList) {
        if (!item) {
            continue;
        }
        if (item->isOrnament()) {
            kn::engraving::Ornament* ornament = toOrnament(item);
            if (!ornament->hasIntervalBelow()) {
                available = false;
                break;
            }
        }
    }

    setIsIntervalBelowAvailable(available);
}

void OrnamentSettingsModel::setIsFullIntervalChoiceAvailable(bool available)
{
    if (available == m_isFullIntervalChoiceAvailable) {
        return;
    }

    m_isFullIntervalChoiceAvailable = available;
    emit isFullIntervalChoiceAvailableChanged(m_isFullIntervalChoiceAvailable);
}

void OrnamentSettingsModel::setIsPerfectStep(bool isPerfect)
{
    if (isPerfect == m_isPerfectStep) {
        return;
    }

    m_isPerfectStep = isPerfect;
    emit isPerfectStepChanged(m_isPerfectStep);
}

void OrnamentSettingsModel::setIsIntervalAboveAvailable(bool available)
{
    if (available == m_isIntervalAboveAvailable) {
        return;
    }

    m_isIntervalAboveAvailable = available;
    emit isIntervalAboveAvailableChanged(m_isIntervalAboveAvailable);
}

void OrnamentSettingsModel::setIsIntervalBelowAvailable(bool available)
{
    if (available == m_isIntervalBelowAvailable) {
        return;
    }

    m_isIntervalBelowAvailable = available;
    emit isIntervalBelowAvailableChanged(m_isIntervalAboveAvailable);
}
