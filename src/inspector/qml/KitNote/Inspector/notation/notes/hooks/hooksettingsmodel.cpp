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
#include "hooksettingsmodel.h"

#include "translation.h"

using namespace kn::inspector;

HookSettingsModel::HookSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_HOOK);
    setTitle(kfw::qtrc("inspector", "Flag")); // internally called "Hook", but "Flag" in SMuFL, so here externally too

    createProperties();
}

void HookSettingsModel::createProperties()
{
    m_offset = buildPointFPropertyItem(kn::engraving::Pid::OFFSET);
}

void HookSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::HOOK);
}

void HookSettingsModel::loadProperties()
{
    loadPropertyItem(m_offset);
    updatemeasurementUnits();
}

void HookSettingsModel::resetProperties()
{
    m_offset->resetToDefault();
}

void HookSettingsModel::onNotationChanged(const kn::engraving::PropertyIdSet& changedPropertyIdSet, const kn::engraving::StyleIdSet&)
{
    if (kfw::contains(changedPropertyIdSet, kn::engraving::Pid::DURATION)
        || kfw::contains(changedPropertyIdSet, kn::engraving::Pid::DURATION_TYPE_WITH_DOTS)
        || kfw::contains(changedPropertyIdSet, kn::engraving::Pid::NO_STEM)
        || kfw::contains(changedPropertyIdSet, kn::engraving::Pid::BEAM_MODE)) {
        // Might affect `isEmpty`
        updateProperties();
    }
}

bool HookSettingsModel::shouldUpdateWhenEmpty() const
{
    return true;
}

PropertyItem* HookSettingsModel::offset() const
{
    return m_offset;
}
