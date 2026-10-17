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
#include "hairpinsettingsmodel.h"

#include "engraving/dom/hairpin.h"

#include "types/commontypes.h"

#include "translation.h"

using namespace kn::inspector;
using namespace kn::engraving;

HairpinSettingsModel::HairpinSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                           IElementRepositoryService* repository)
    : HairpinLineSettingsModel(parent, iocCtx, repository, HairpinLineSettingsModel::Unknown)
{
    setModelType(InspectorModelType::TYPE_HAIRPIN);
    setTitle(kfw::qtrc("inspector", "Hairpin"));
    setIcon(kfw::ui::IconCode::Code::HAIRPIN);

    createProperties();
}

PropertyItem* HairpinSettingsModel::isNienteCircleVisible() const
{
    return m_isNienteCircleVisible;
}

PropertyItem* HairpinSettingsModel::height() const
{
    return m_height;
}

PropertyItem* HairpinSettingsModel::continuousHeight() const
{
    return m_continuousHeight;
}

void HairpinSettingsModel::createProperties()
{
    HairpinLineSettingsModel::createProperties();

    m_isNienteCircleVisible = buildPropertyItem(kn::engraving::Pid::HAIRPIN_CIRCLEDTIP);
    m_height = buildPropertyItem(kn::engraving::Pid::HAIRPIN_HEIGHT);
    m_continuousHeight = buildPropertyItem(kn::engraving::Pid::HAIRPIN_CONT_HEIGHT);

    isLineVisible()->setIsVisible(false);
    allowDiagonal()->setIsVisible(true);
    placement()->setIsVisible(true);
}

void HairpinSettingsModel::loadProperties()
{
    HairpinLineSettingsModel::loadProperties();

    const static PropertyIdSet propertyIdSet {
        Pid::HAIRPIN_CIRCLEDTIP,
        Pid::HAIRPIN_HEIGHT,
        Pid::HAIRPIN_CONT_HEIGHT,
        Pid::SNAP_BEFORE,
        Pid::SNAP_AFTER
    };

    loadProperties(propertyIdSet);
}

void HairpinSettingsModel::resetProperties()
{
    HairpinLineSettingsModel::resetProperties();

    m_isNienteCircleVisible->resetToDefault();
    m_height->resetToDefault();
    m_continuousHeight->resetToDefault();
}

void HairpinSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::HAIRPIN, [](const kn::engraving::EngravingItem* element) -> bool {
        const kn::engraving::Hairpin* hairpin = kn::engraving::toHairpin(
            element);

        if (!hairpin) {
            return false;
        }

        return hairpin->hairpinType() == kn::engraving::HairpinType::CRESC_HAIRPIN || hairpin->hairpinType() == kn::engraving::HairpinType::DIM_HAIRPIN;
    });
}

void HairpinSettingsModel::onNotationChanged(const PropertyIdSet& changedPropertyIdSet, const StyleIdSet& changedStyleIdSet)
{
    HairpinLineSettingsModel::onNotationChanged(changedPropertyIdSet, changedStyleIdSet);
    loadProperties(changedPropertyIdSet);
}

void HairpinSettingsModel::loadProperties(const PropertyIdSet& propertyIdSet)
{
    if (kfw::contains(propertyIdSet, Pid::HAIRPIN_CIRCLEDTIP)) {
        loadPropertyItem(m_isNienteCircleVisible);
    }

    if (kfw::contains(propertyIdSet, Pid::HAIRPIN_HEIGHT)) {
        loadPropertyItem(m_height, formatDoubleFunc);
    }

    if (kfw::contains(propertyIdSet, Pid::HAIRPIN_CONT_HEIGHT)) {
        loadPropertyItem(m_continuousHeight, formatDoubleFunc);
    }
}
