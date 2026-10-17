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

#include "hairpinlinesettingsmodel.h"

#include "engraving/dom/hairpin.h"

#include "translation.h"

#include "ui/view/iconcodes.h"

using namespace kn::inspector;

using IconCode = kfw::ui::IconCode::Code;

HairpinLineSettingsModel::HairpinLineSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                                   IElementRepositoryService* repository, HairpinLineType lineType)
    : TextLineSettingsModel(parent, iocCtx, repository)
{
    if (lineType == Diminuendo) {
        setModelType(InspectorModelType::TYPE_DIMINUENDO);
        setTitle(kfw::qtrc("inspector", "Diminuendo"));
        setIcon(kfw::ui::IconCode::Code::DIMINUENDO);
    } else {
        setModelType(InspectorModelType::TYPE_CRESCENDO);
        setTitle(kfw::qtrc("inspector", "Crescendo"));
        setIcon(kfw::ui::IconCode::Code::CRESCENDO);
    }

    m_hairpinType = lineType == Crescendo ? engraving::HairpinType::CRESC_LINE : engraving::HairpinType::DIM_LINE;

    createProperties();
}

PropertyItem* HairpinLineSettingsModel::snapBefore() const
{
    return m_snapBefore;
}

PropertyItem* HairpinLineSettingsModel::snapAfter() const
{
    return m_snapAfter;
}

void HairpinLineSettingsModel::createProperties()
{
    TextLineSettingsModel::createProperties();

    m_snapBefore = buildPropertyItem(kn::engraving::Pid::SNAP_BEFORE);
    m_snapAfter = buildPropertyItem(kn::engraving::Pid::SNAP_AFTER);

    isLineVisible()->setIsVisible(true);
    allowDiagonal()->setIsVisible(false);
    placement()->setIsVisible(true);
}

void HairpinLineSettingsModel::loadProperties()
{
    TextLineSettingsModel::loadProperties();

    loadPropertyItem(m_snapBefore);
    loadPropertyItem(m_snapAfter);
}

void HairpinLineSettingsModel::resetProperties()
{
    TextLineSettingsModel::resetProperties();

    m_snapBefore->resetToDefault();
    m_snapAfter->resetToDefault();
}

void HairpinLineSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::HAIRPIN, [this](const kn::engraving::EngravingItem* element) -> bool {
        const kn::engraving::Hairpin* hairpin = kn::engraving::toHairpin(
            element);

        if (!hairpin) {
            return false;
        }

        return hairpin->hairpinType() == m_hairpinType;
    });
}
