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

#include "tupletsettingsmodel.h"

#include "types/commontypes.h"

#include "translation.h"

using namespace kn::inspector;

using Icon = kfw::ui::IconCode::Code;

TupletSettingsModel::TupletSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository, kn::engraving::ElementType::TUPLET)
{
    setModelType(InspectorModelType::TYPE_TUPLET);
    setTitle(kfw::qtrc("inspector", "Tuplet"));
    setIcon(kfw::ui::IconCode::Code::TUPLET_NUMBER_WITH_BRACKETS);
    createProperties();
}

PropertyItem* TupletSettingsModel::directionType() const
{
    return m_directionType;
}

PropertyItem* TupletSettingsModel::numberType() const
{
    return m_numberType;
}

PropertyItem* TupletSettingsModel::bracketType() const
{
    return m_bracketType;
}

PropertyItem* TupletSettingsModel::lineThickness() const
{
    return m_lineThickness;
}

QVariantList TupletSettingsModel::possibleNumberTypes() const
{
    using Type = kn::engraving::TupletNumberType;

    QVariantList types {
        object(Type::SHOW_NUMBER, kfw::qtrc("inspector", "Number", "tuplet number type")),
        object(Type::SHOW_RELATION, kfw::qtrc("inspector", "Ratio", "tuplet number type")),
        object(Type::NO_TEXT, kfw::qtrc("inspector", "None", "tuplet number type"))
    };

    return types;
}

QVariantList TupletSettingsModel::possibleBracketTypes() const
{
    using Type = kn::engraving::TupletBracketType;

    QVariantList types {
        object(Type::AUTO_BRACKET, kfw::qtrc("inspector", "Auto", "tuplet bracket type")),
        object(Type::SHOW_BRACKET, kfw::qtrc("inspector", "Bracket", "tuplet bracket type"), Icon::TUPLET_NUMBER_WITH_BRACKETS),
        object(Type::SHOW_NO_BRACKET, kfw::qtrc("inspector", "None", "tuplet bracket type"), Icon::TUPLET_NUMBER_ONLY)
    };

    return types;
}

void TupletSettingsModel::createProperties()
{
    m_directionType = buildPropertyItem(kn::engraving::Pid::DIRECTION);
    m_numberType = buildPropertyItem(kn::engraving::Pid::NUMBER_TYPE);
    m_bracketType = buildPropertyItem(kn::engraving::Pid::BRACKET_TYPE);
    m_lineThickness = buildPropertyItem(kn::engraving::Pid::LINE_WIDTH);
}

void TupletSettingsModel::loadProperties()
{
    loadPropertyItem(m_directionType);
    loadPropertyItem(m_numberType);
    loadPropertyItem(m_bracketType);
    loadPropertyItem(m_lineThickness, formatDoubleFunc);
}

void TupletSettingsModel::resetProperties()
{
    m_directionType->resetToDefault();
    m_numberType->resetToDefault();
    m_bracketType->resetToDefault();
    m_lineThickness->resetToDefault();
}
