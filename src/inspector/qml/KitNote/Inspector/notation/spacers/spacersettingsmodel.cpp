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
#include "spacersettingsmodel.h"

#include "dataformatter.h"

#include "translation.h"

using namespace kn::inspector;
using namespace kn::engraving;

SpacerSettingsModel::SpacerSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_SPACER);
    setTitle(kfw::qtrc("inspector", "Spacer"));
    setIcon(kfw::ui::IconCode::Code::SPACER);
    createProperties();
}

void SpacerSettingsModel::createProperties()
{
    m_spacerHeight = buildPropertyItem(Pid::SPACE);
}

void SpacerSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(ElementType::SPACER);
}

void SpacerSettingsModel::loadProperties()
{
    loadPropertyItem(m_spacerHeight, formatDoubleFunc);
}

void SpacerSettingsModel::resetProperties()
{
    m_spacerHeight->resetToDefault();
}

void SpacerSettingsModel::onNotationChanged(const PropertyIdSet& changedPropertyIdSet, const StyleIdSet&)
{
    if (kfw::contains(changedPropertyIdSet, Pid::SPACE)) {
        loadPropertyItem(m_spacerHeight, formatDoubleFunc);
    }
}

PropertyItem* SpacerSettingsModel::spacerHeight() const
{
    return m_spacerHeight;
}
