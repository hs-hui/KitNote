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
#include "beammodesmodel.h"

using namespace kn::inspector;

BeamModesModel::BeamModesModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    createProperties();
}

void BeamModesModel::createProperties()
{
    m_mode = buildPropertyItem(kn::engraving::Pid::BEAM_MODE);
    m_isFeatheringAvailable = buildPropertyItem(kn::engraving::Pid::DURATION_TYPE_WITH_DOTS, [](const kn::engraving::Pid, const QVariant&) {
    });                                                                                                                                         //@note readonly property, there is no need to modify it
}

void BeamModesModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::CHORD) << m_repository->findElementsByType(
        kn::engraving::ElementType::REST);
}

void BeamModesModel::loadProperties()
{
    loadPropertyItem(m_mode);

    loadPropertyItem(m_isFeatheringAvailable, [](const QVariant& elementPropertyValue) -> QVariant {
        QVariantMap map = elementPropertyValue.toMap();
        if (map.isEmpty()) {
            return false;
        }

        kn::engraving::DurationType durationType = static_cast<kn::engraving::DurationType>(map["type"].toInt());

        switch (durationType) {
            case kn::engraving::DurationType::V_INVALID:
            case kn::engraving::DurationType::V_MEASURE:
            case kn::engraving::DurationType::V_ZERO:
            case kn::engraving::DurationType::V_LONG:
            case kn::engraving::DurationType::V_BREVE:
            case kn::engraving::DurationType::V_WHOLE:
            case kn::engraving::DurationType::V_HALF:
            case kn::engraving::DurationType::V_QUARTER:
            case kn::engraving::DurationType::V_EIGHTH:
                return false;

            default:
                return true;
        }
    });
}

void BeamModesModel::resetProperties()
{
    m_mode->resetToDefault();
}

PropertyItem* BeamModesModel::mode() const
{
    return m_mode;
}

PropertyItem* BeamModesModel::isFeatheringAvailable() const
{
    return m_isFeatheringAvailable;
}
