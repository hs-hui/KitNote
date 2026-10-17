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

#include "gradualtempochangesettingsmodel.h"

using namespace kn::inspector;
using namespace kn::engraving;

GradualTempoChangeSettingsModel::GradualTempoChangeSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                                                 IElementRepositoryService* repository)
    : TextLineSettingsModel(parent, iocCtx, repository, ElementType::GRADUAL_TEMPO_CHANGE)
{
    setModelType(InspectorModelType::TYPE_GRADUAL_TEMPO_CHANGE);
    setTitle(kfw::qtrc("inspector", "Tempo change"));
    setIcon(kfw::ui::IconCode::Code::TEMPO_CHANGE);

    createProperties();
}

void GradualTempoChangeSettingsModel::createProperties()
{
    TextLineSettingsModel::createProperties();

    isLineVisible()->setIsVisible(true);
    allowDiagonal()->setIsVisible(true);
    placement()->setIsVisible(true);

    m_snapBefore = buildPropertyItem(kn::engraving::Pid::SNAP_BEFORE);
    m_snapAfter = buildPropertyItem(kn::engraving::Pid::SNAP_AFTER);
}

void GradualTempoChangeSettingsModel::loadProperties()
{
    TextLineSettingsModel::loadProperties();

    loadPropertyItem(m_snapBefore);
    loadPropertyItem(m_snapAfter);
}

void GradualTempoChangeSettingsModel::resetProperties()
{
    TextLineSettingsModel::resetProperties();

    m_snapBefore->resetToDefault();
    m_snapAfter->resetToDefault();
}

PropertyItem* GradualTempoChangeSettingsModel::snapBefore() const
{
    return m_snapBefore;
}

PropertyItem* GradualTempoChangeSettingsModel::snapAfter() const
{
    return m_snapAfter;
}
