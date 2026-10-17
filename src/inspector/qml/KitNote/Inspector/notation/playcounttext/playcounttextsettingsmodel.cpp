/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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
#include "playcounttextsettingsmodel.h"

#include "translation.h"

using namespace kn::inspector;
using namespace kn::engraving;

PlayCountTextSettingsModel::PlayCountTextSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                                       IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_PLAY_COUNT_TEXT);
    setTitle(kfw::qtrc("inspector", "Play count text"));
    setIcon(kfw::ui::IconCode::Code::SECTION_BREAK);
    createProperties();
}

void PlayCountTextSettingsModel::createProperties()
{
    m_playCountText = buildPropertyItem(Pid::PLAY_COUNT_TEXT);
    m_playCountTextSetting = buildPropertyItem(Pid::PLAY_COUNT_TEXT_SETTING);
}

void PlayCountTextSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(ElementType::PLAY_COUNT_TEXT);
}

void PlayCountTextSettingsModel::loadProperties()
{
    static const PropertyIdSet propertyIdSet {
        Pid::PLAY_COUNT_TEXT,
        Pid::PLAY_COUNT_TEXT_SETTING,
    };

    loadProperties(propertyIdSet);
}

void PlayCountTextSettingsModel::resetProperties()
{
    m_playCountText->resetToDefault();
    m_playCountTextSetting->resetToDefault();
}

void PlayCountTextSettingsModel::onNotationChanged(const kn::engraving::PropertyIdSet& changedPropertyIdSet,
                                                   const kn::engraving::StyleIdSet&)
{
    loadProperties(changedPropertyIdSet);
}

void PlayCountTextSettingsModel::loadProperties(const kn::engraving::PropertyIdSet& propertyIdSet)
{
    if (kfw::contains(propertyIdSet, Pid::PLAY_COUNT_TEXT)) {
        loadPropertyItem(m_playCountText, [](const QVariant& elementPropertyValue) -> QVariant {
            return elementPropertyValue.toString();
        });
    }
    if (kfw::contains(propertyIdSet, Pid::PLAY_COUNT_TEXT_SETTING)) {
        loadPropertyItem(m_playCountTextSetting, [](const QVariant& elementPropertyValue) -> QVariant {
            return elementPropertyValue.toInt();
        });
    }
}

PropertyItem* PlayCountTextSettingsModel::playCountText() const
{
    return m_playCountText;
}

PropertyItem* PlayCountTextSettingsModel::playCountTextSetting() const
{
    return m_playCountTextSetting;
}
