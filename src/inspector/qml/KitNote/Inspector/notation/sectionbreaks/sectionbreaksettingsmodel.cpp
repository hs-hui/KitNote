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
#include "sectionbreaksettingsmodel.h"

#include "translation.h"
#include "dataformatter.h"

using namespace kn::inspector;

SectionBreakSettingsModel::SectionBreakSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                                     IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_SECTIONBREAK);
    setTitle(kfw::qtrc("inspector", "Section break"));
    setIcon(kfw::ui::IconCode::Code::SECTION_BREAK);
    createProperties();
}

void SectionBreakSettingsModel::createProperties()
{
    m_shouldStartWithLongInstrNames = buildPropertyItem(kn::engraving::Pid::START_WITH_LONG_NAMES);
    m_shouldResetBarNums = buildPropertyItem(kn::engraving::Pid::START_WITH_MEASURE_ONE);
    m_pauseDuration = buildPropertyItem(kn::engraving::Pid::PAUSE);
    m_firstSystemIndent = buildPropertyItem(kn::engraving::Pid::FIRST_SYSTEM_INDENTATION);
    m_showCourtesySignatures = buildPropertyItem(kn::engraving::Pid::SHOW_COURTESY);
}

void SectionBreakSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::LAYOUT_BREAK);
}

void SectionBreakSettingsModel::loadProperties()
{
    loadPropertyItem(m_shouldStartWithLongInstrNames);
    loadPropertyItem(m_shouldResetBarNums);
    loadPropertyItem(m_pauseDuration, formatDoubleFunc);
    loadPropertyItem(m_firstSystemIndent);
    loadPropertyItem(m_showCourtesySignatures);
}

void SectionBreakSettingsModel::resetProperties()
{
    m_shouldStartWithLongInstrNames->resetToDefault();
    m_shouldResetBarNums->resetToDefault();
    m_pauseDuration->resetToDefault();
    m_firstSystemIndent->resetToDefault();
    m_showCourtesySignatures->resetToDefault();
}

PropertyItem* SectionBreakSettingsModel::shouldStartWithLongInstrNames() const
{
    return m_shouldStartWithLongInstrNames;
}

PropertyItem* SectionBreakSettingsModel::shouldResetBarNums() const
{
    return m_shouldResetBarNums;
}

PropertyItem* SectionBreakSettingsModel::pauseDuration() const
{
    return m_pauseDuration;
}

PropertyItem* SectionBreakSettingsModel::firstSystemIndent() const
{
    return m_firstSystemIndent;
}

PropertyItem* SectionBreakSettingsModel::showCourtesySignatures() const
{
    return m_showCourtesySignatures;
}
