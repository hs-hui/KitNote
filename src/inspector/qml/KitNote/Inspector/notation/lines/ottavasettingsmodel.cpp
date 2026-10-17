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

#include "ottavasettingsmodel.h"

#include "translation.h"

#include "ui/view/iconcodes.h"

using namespace kn::inspector;

using IconCode = kfw::ui::IconCode::Code;

OttavaSettingsModel::OttavaSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : TextLineSettingsModel(parent, iocCtx, repository, kn::engraving::ElementType::OTTAVA)
{
    setTitle(kfw::qtrc("inspector", "Ottava"));
    setModelType(InspectorModelType::TYPE_OTTAVA);
    setIcon(kfw::ui::IconCode::Code::OTTAVA);

    createProperties();
}

PropertyItem* OttavaSettingsModel::ottavaType() const
{
    return m_ottavaType;
}

PropertyItem* OttavaSettingsModel::showNumbersOnly() const
{
    return m_showNumbersOnly;
}

QVariantList OttavaSettingsModel::possibleOttavaTypes() const
{
    QMap<kn::engraving::OttavaType, QString> ottavaTypes {
        { kn::engraving::OttavaType::OTTAVA_8VA, kfw::qtrc("inspector", "8va alta") },
        { kn::engraving::OttavaType::OTTAVA_8VB, kfw::qtrc("inspector", "8va bassa") },
        { kn::engraving::OttavaType::OTTAVA_15MA, kfw::qtrc("inspector", "15ma alta") },
        { kn::engraving::OttavaType::OTTAVA_15MB, kfw::qtrc("inspector", "15ma bassa") },
        { kn::engraving::OttavaType::OTTAVA_22MA, kfw::qtrc("inspector", "22ma alta") },
        { kn::engraving::OttavaType::OTTAVA_22MB, kfw::qtrc("inspector", "22ma bassa") }
    };

    QVariantList result;

    for (kn::engraving::OttavaType type : ottavaTypes.keys()) {
        QVariantMap obj;
        obj["value"] = static_cast<int>(type);
        obj["text"] = ottavaTypes[type];

        result << obj;
    }

    return result;
}

void OttavaSettingsModel::updateStartAndEndHookTypes()
{
    setPossibleStartHookTypes({});

    static const QList<HookTypeInfo> endHookTypes {
        { kn::engraving::HookType::NONE, IconCode::LINE_NORMAL, kfw::qtrc("inspector", "Normal", "hook type") },
        { kn::engraving::HookType::HOOK_90, IconCode::LINE_WITH_END_HOOK, kfw::qtrc("inspector", "Hooked 90°", "hook type") },
        { kn::engraving::HookType::HOOK_45, IconCode::LINE_WITH_ANGLED_END_HOOK, kfw::qtrc("inspector", "Hooked 45°", "hook type") },
        { kn::engraving::HookType::ARROW, IconCode::LINE_ARROW_RIGHT, kfw::qtrc("inspector", "Line arrow", "hook type") },
        { kn::engraving::HookType::ARROW_FILLED, IconCode::FILLED_ARROW_RIGHT, kfw::qtrc("inspector", "Filled arrow", "hook type") }
    };

    setPossibleEndHookTypes(endHookTypes);
}

void OttavaSettingsModel::createProperties()
{
    TextLineSettingsModel::createProperties();

    m_ottavaType = buildPropertyItem(kn::engraving::Pid::OTTAVA_TYPE);
    m_showNumbersOnly = buildPropertyItem(kn::engraving::Pid::NUMBERS_ONLY);

    isLineVisible()->setIsVisible(true);
    allowDiagonal()->setIsVisible(true);
    placement()->setIsVisible(false);
}

void OttavaSettingsModel::loadProperties()
{
    TextLineSettingsModel::loadProperties();

    loadPropertyItem(m_ottavaType);
    loadPropertyItem(m_showNumbersOnly);
    updateStartAndEndHookTypes();
}

void OttavaSettingsModel::resetProperties()
{
    TextLineSettingsModel::resetProperties();

    m_ottavaType->resetToDefault();
    m_showNumbersOnly->resetToDefault();
}
