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
#include "fermatasettingsmodel.h"

#include "translation.h"

using namespace kn::inspector;

FermataSettingsModel::FermataSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                           IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_FERMATA);
    setTitle(kfw::qtrc("inspector", "Fermata"));
    setIcon(kfw::ui::IconCode::Code::FERMATA);
    createProperties();
}

void FermataSettingsModel::createProperties()
{
    m_placementType = buildPropertyItem(kn::engraving::Pid::PLACEMENT);
}

void FermataSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::FERMATA);
}

void FermataSettingsModel::loadProperties()
{
    loadPropertyItem(m_placementType);
}

void FermataSettingsModel::resetProperties()
{
    m_placementType->resetToDefault();
}

PropertyItem* FermataSettingsModel::placementType() const
{
    return m_placementType;
}
