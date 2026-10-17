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
#include "markersettingsmodel.h"
#include "engraving/types/typesconv.h"

#include "translation.h"

using namespace kn::inspector;

MarkerSettingsModel::MarkerSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_MARKER);
    setTitle(kfw::qtrc("inspector", "Marker"));
    setIcon(kfw::ui::IconCode::Code::MARKER);
    createProperties();
}

void MarkerSettingsModel::createProperties()
{
    m_type = buildPropertyItem(kn::engraving::Pid::MARKER_TYPE);
    m_label = buildPropertyItem(kn::engraving::Pid::LABEL);
    m_position = buildPropertyItem(kn::engraving::Pid::POSITION);
    m_centerOnSymbol = buildPropertyItem(kn::engraving::Pid::MARKER_CENTER_ON_SYMBOL);
}

void MarkerSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::MARKER);
}

void MarkerSettingsModel::loadProperties()
{
    loadPropertyItem(m_type);
    loadPropertyItem(m_label);
    loadPropertyItem(m_position);
    loadPropertyItem(m_centerOnSymbol);
}

void MarkerSettingsModel::resetProperties()
{
    m_type->resetToDefault();
    m_label->resetToDefault();
    m_position->resetToDefault();
    m_centerOnSymbol->resetToDefault();
}

PropertyItem* MarkerSettingsModel::type() const
{
    return m_type;
}

PropertyItem* MarkerSettingsModel::label() const
{
    return m_label;
}

PropertyItem* MarkerSettingsModel::position() const
{
    return m_position;
}

PropertyItem* MarkerSettingsModel::centerOnSymbol() const
{
    return m_centerOnSymbol;
}

QString MarkerSettingsModel::markerTypeName() const
{
    if (!type()) {
        return "";
    }

    if (type()->isUndefined()) {
        return "--";
    }

    return engraving::TConv::translatedUserName(type()->value().value<engraving::MarkerType>());
}
