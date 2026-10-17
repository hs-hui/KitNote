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
#include "breathplaybackmodel.h"

#include "dataformatter.h"

#include "translation.h"

using namespace kn::inspector;

BreathPlaybackModel::BreathPlaybackModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setTitle(kfw::qtrc("inspector", "Breaths & pauses"));
    setModelType(InspectorModelType::TYPE_BREATH);

    createProperties();
}

void BreathPlaybackModel::createProperties()
{
    m_pauseTime = buildPropertyItem(kn::engraving::Pid::PAUSE);
}

void BreathPlaybackModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::BREATH);
}

void BreathPlaybackModel::loadProperties()
{
    loadPropertyItem(m_pauseTime, formatDoubleFunc);
}

void BreathPlaybackModel::resetProperties()
{
    m_pauseTime->resetToDefault();
}

PropertyItem* BreathPlaybackModel::pauseTime() const
{
    return m_pauseTime;
}
