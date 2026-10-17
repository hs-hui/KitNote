/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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
#include "restbeamsettingsmodel.h"

#include "translation.h"

using namespace kn::inspector;
using namespace kn::engraving;

RestBeamSettingsModel::RestBeamSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                             IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_BEAM);
    setTitle(kfw::qtrc("inspector", "Beam"));

    m_beamModesModel = new BeamModesModel(this, iocCtx, repository);
    m_beamModesModel->init();

    connect(m_beamModesModel->mode(), &PropertyItem::propertyModified, this, &AbstractInspectorModel::requestReloadPropertyItems);
}

QObject* RestBeamSettingsModel::beamModesModel() const
{
    return m_beamModesModel;
}

void RestBeamSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::REST);
}

void RestBeamSettingsModel::onCurrentNotationChanged()
{
    m_beamModesModel->onCurrentNotationChanged();
}
