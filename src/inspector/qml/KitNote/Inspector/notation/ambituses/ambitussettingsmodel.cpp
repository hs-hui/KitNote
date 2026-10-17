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
#include "ambitussettingsmodel.h"

#include "dataformatter.h"

#include "translation.h"

using namespace kn::inspector;

AmbitusSettingsModel::AmbitusSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                           IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_AMBITUS);
    setTitle(kfw::qtrc("inspector", "Ambitus"));
    setIcon(kfw::ui::IconCode::Code::AMBITUS);
    createProperties();
}

void AmbitusSettingsModel::createProperties()
{
    m_noteheadGroup = buildPropertyItem(kn::engraving::Pid::HEAD_GROUP);
    m_noteheadType = buildPropertyItem(kn::engraving::Pid::HEAD_TYPE);

    m_topTpc = buildPropertyItem(kn::engraving::Pid::TPC1);
    m_bottomTpc = buildPropertyItem(kn::engraving::Pid::FBPARENTHESIS1);
    m_topOctave = buildPropertyItem(kn::engraving::Pid::FBPARENTHESIS3);
    m_bottomOctave = buildPropertyItem(kn::engraving::Pid::FBPARENTHESIS4);

    m_topPitch = buildPropertyItem(kn::engraving::Pid::PITCH, [this](const kn::engraving::Pid pid, const QVariant& newValue) {
        onPropertyValueChanged(pid, newValue);

        emit requestReloadPropertyItems();
    });

    m_bottomPitch = buildPropertyItem(kn::engraving::Pid::FBPARENTHESIS2, [this](const kn::engraving::Pid pid, const QVariant& newValue) {
        onPropertyValueChanged(pid, newValue);

        emit requestReloadPropertyItems();
    });

    m_direction = buildPropertyItem(kn::engraving::Pid::MIRROR_HEAD);
    m_lineThickness = buildPropertyItem(kn::engraving::Pid::LINE_WIDTH);
}

void AmbitusSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::AMBITUS);
}

void AmbitusSettingsModel::loadProperties()
{
    loadPropertyItem(m_noteheadGroup);
    loadPropertyItem(m_noteheadType);

    loadPropertyItem(m_topTpc);
    loadPropertyItem(m_bottomTpc);
    loadPropertyItem(m_topOctave);
    loadPropertyItem(m_bottomOctave);
    loadPropertyItem(m_topPitch);
    loadPropertyItem(m_bottomPitch);

    loadPropertyItem(m_direction);
    loadPropertyItem(m_lineThickness, formatDoubleFunc);
}

void AmbitusSettingsModel::resetProperties()
{
    m_noteheadGroup->resetToDefault();
    m_noteheadType->resetToDefault();

    m_direction->resetToDefault();
    m_lineThickness->resetToDefault();

    m_topTpc->resetToDefault();
    m_bottomTpc->resetToDefault();
    m_topPitch->resetToDefault();
    m_bottomPitch->resetToDefault();
}

void AmbitusSettingsModel::matchRangesToStaff()
{
    m_topTpc->resetToDefault();
    m_bottomTpc->resetToDefault();
    m_topPitch->resetToDefault();
    m_bottomPitch->resetToDefault();
}

PropertyItem* AmbitusSettingsModel::noteheadGroup() const
{
    return m_noteheadGroup;
}

PropertyItem* AmbitusSettingsModel::noteheadType() const
{
    return m_noteheadType;
}

PropertyItem* AmbitusSettingsModel::topTpc() const
{
    return m_topTpc;
}

PropertyItem* AmbitusSettingsModel::bottomTpc() const
{
    return m_bottomTpc;
}

PropertyItem* AmbitusSettingsModel::topOctave() const
{
    return m_topOctave;
}

PropertyItem* AmbitusSettingsModel::bottomOctave() const
{
    return m_bottomOctave;
}

PropertyItem* AmbitusSettingsModel::direction() const
{
    return m_direction;
}

PropertyItem* AmbitusSettingsModel::lineThickness() const
{
    return m_lineThickness;
}

PropertyItem* AmbitusSettingsModel::topPitch() const
{
    return m_topPitch;
}

PropertyItem* AmbitusSettingsModel::bottomPitch() const
{
    return m_bottomPitch;
}
