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
#include "timesignaturesettingsmodel.h"

#include "dataformatter.h"
#include "translation.h"

#include "engraving/dom/layoutbreak.h"
#include "engraving/dom/measure.h"

using namespace kn::inspector;

TimeSignatureSettingsModel::TimeSignatureSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                                       IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_TIME_SIGNATURE);
    setTitle(kfw::qtrc("inspector", "Time signature"));
    setIcon(kfw::ui::IconCode::Code::TIME_SIGNATURE);
    createProperties();
}

void TimeSignatureSettingsModel::createProperties()
{
    m_horizontalScale = buildPropertyItem(kn::engraving::Pid::SCALE,
                                          [this](const kn::engraving::Pid pid, const QVariant& newValue) {
        onPropertyValueChanged(pid, QSizeF(newValue.toDouble(), m_verticalScale->value().toDouble()));
    },
                                          [this](const kn::engraving::Sid sid, const QVariant& newValue) {
        updateStyleValue(sid, QSizeF(newValue.toDouble(), m_verticalScale->value().toDouble()));
        emit requestReloadPropertyItems();
    });

    m_verticalScale = buildPropertyItem(kn::engraving::Pid::SCALE,
                                        [this](const kn::engraving::Pid pid, const QVariant& newValue) {
        onPropertyValueChanged(pid, QSizeF(m_horizontalScale->value().toDouble(), newValue.toDouble()));
    },
                                        [this](const kn::engraving::Sid sid, const QVariant& newValue) {
        updateStyleValue(sid, QSizeF(m_horizontalScale->value().toDouble(), newValue.toDouble()));
        emit requestReloadPropertyItems();
    });

    m_shouldShowCourtesy = buildPropertyItem(kn::engraving::Pid::SHOW_COURTESY);
}

void TimeSignatureSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::TIMESIG);
}

void TimeSignatureSettingsModel::loadProperties()
{
    loadPropertyItem(m_horizontalScale, [](const QVariant& elementPropertyValue) -> QVariant {
        return kfw::DataFormatter::roundDouble(elementPropertyValue.value<QSizeF>().width());
    });

    loadPropertyItem(m_verticalScale, [](const QVariant& elementPropertyValue) -> QVariant {
        return kfw::DataFormatter::roundDouble(elementPropertyValue.value<QSizeF>().height());
    });

    loadPropertyItem(m_shouldShowCourtesy);

    bool enableCourtesy = true;

    for (const kn::engraving::EngravingItem* element : m_elementList) {
        if (element->generated()) {
            m_isGenerated = true;
        }

        const engraving::Measure* measure = element->findMeasure();
        const engraving::Measure* prevMeasure = measure ? measure->prevMeasure() : nullptr;
        const engraving::LayoutBreak* sectionBreak = prevMeasure ? prevMeasure->sectionBreakElement() : nullptr;
        if (sectionBreak && !sectionBreak->showCourtesy()) {
            enableCourtesy = false;
            break;
        }
    }

    m_shouldShowCourtesy->setIsEnabled(!m_isGenerated && enableCourtesy);
    m_horizontalScale->setIsEnabled(!m_isGenerated);
    m_verticalScale->setIsEnabled(!m_isGenerated);
}

void TimeSignatureSettingsModel::resetProperties()
{
    m_horizontalScale->resetToDefault();
    m_verticalScale->resetToDefault();
    m_shouldShowCourtesy->resetToDefault();
}

void TimeSignatureSettingsModel::showTimeSignatureProperties()
{
    dispatcher()->dispatch("time-signature-properties");
}

PropertyItem* TimeSignatureSettingsModel::horizontalScale() const
{
    return m_horizontalScale;
}

PropertyItem* TimeSignatureSettingsModel::verticalScale() const
{
    return m_verticalScale;
}

PropertyItem* TimeSignatureSettingsModel::shouldShowCourtesy() const
{
    return m_shouldShowCourtesy;
}
