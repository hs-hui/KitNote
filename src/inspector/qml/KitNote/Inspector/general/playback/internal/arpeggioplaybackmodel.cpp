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
#include "arpeggioplaybackmodel.h"

#include "dataformatter.h"

#include "translation.h"

using namespace kn::inspector;

ArpeggioPlaybackModel::ArpeggioPlaybackModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                             IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setTitle(kfw::qtrc("inspector", "Arpeggio"));
    setModelType(InspectorModelType::TYPE_ARPEGGIO);

    createProperties();
}

void ArpeggioPlaybackModel::createProperties()
{
    m_stretch = buildPropertyItem(kn::engraving::Pid::TIME_STRETCH);
}

void ArpeggioPlaybackModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::ARPEGGIO);
}

void ArpeggioPlaybackModel::loadProperties()
{
    loadPropertyItem(m_stretch, formatDoubleFunc);
}

void ArpeggioPlaybackModel::resetProperties()
{
    m_stretch->resetToDefault();
}

PropertyItem* ArpeggioPlaybackModel::stretch() const
{
    return m_stretch;
}
