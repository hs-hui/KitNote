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
#include "pedalsettingsmodel.h"

#include "translation.h"
#include "ui/view/iconcodes.h"

#include "engraving/dom/pedal.h"

using namespace kn::inspector;

using IconCode = kfw::ui::IconCode::Code;

PedalSettingsModel::PedalSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : TextLineSettingsModel(parent, iocCtx, repository, kn::engraving::ElementType::PEDAL)
{
    setModelType(InspectorModelType::TYPE_PEDAL);
    setTitle(kfw::qtrc("inspector", "Pedal"));
    setIcon(kfw::ui::IconCode::Code::PEDAL_MARKING);

    createProperties();
}

PropertyItem* PedalSettingsModel::lineType() const
{
    return m_lineType;
}

bool PedalSettingsModel::isChangingLineVisibilityAllowed() const
{
    return m_rosetteHookSelected;
}

void PedalSettingsModel::updateStartAndEndHookTypes()
{
    static const QList<HookTypeInfo> startHookTypes {
        { kn::engraving::HookType::NONE, IconCode::LINE_NORMAL, kfw::qtrc("inspector", "Normal", "hook type") },
        { kn::engraving::HookType::HOOK_90, IconCode::LINE_WITH_START_HOOK, kfw::qtrc("inspector", "Hooked 90°", "hook type") },
        { kn::engraving::HookType::HOOK_45, IconCode::LINE_WITH_ANGLED_START_HOOK, kfw::qtrc("inspector", "Hooked 45°", "hook type") },
        { kn::engraving::HookType::HOOK_90T, IconCode::LINE_WITH_T_LINE_START_HOOK,
          kfw::qtrc("inspector", "Hooked 90° T-style", "hook type") },
        { kn::engraving::HookType::ARROW, IconCode::LINE_ARROW_LEFT, kfw::qtrc("inspector", "Line arrow", "hook type") },
        { kn::engraving::HookType::ARROW_FILLED, IconCode::FILLED_ARROW_LEFT, kfw::qtrc("inspector", "Filled arrow", "hook type") }
    };

    setPossibleStartHookTypes(startHookTypes);

    static const QList<HookTypeInfo> endHookTypes {
        { kn::engraving::HookType::NONE, IconCode::LINE_NORMAL, kfw::qtrc("inspector", "Normal", "hook type") },
        { kn::engraving::HookType::ROSETTE, IconCode::LINE_PEDAL_STAR_ENDING, kfw::qtrc("inspector", "Asterisk", "hook type") },
        { kn::engraving::HookType::HOOK_90, IconCode::LINE_WITH_END_HOOK, kfw::qtrc("inspector", "Hooked 90°", "hook type") },
        { kn::engraving::HookType::HOOK_45, IconCode::LINE_WITH_ANGLED_END_HOOK, kfw::qtrc("inspector", "Hooked 45°", "hook type") },
        { kn::engraving::HookType::HOOK_90T, IconCode::LINE_WITH_T_LIKE_END_HOOK,
          kfw::qtrc("inspector", "Hooked 90° T-style", "hook type") },
        { kn::engraving::HookType::ARROW, IconCode::LINE_ARROW_RIGHT, kfw::qtrc("inspector", "Line arrow", "hook type") },
        { kn::engraving::HookType::ARROW_FILLED, IconCode::FILLED_ARROW_RIGHT, kfw::qtrc("inspector", "Filled arrow", "hook type") }
    };

    setPossibleEndHookTypes(endHookTypes);
}

bool PedalSettingsModel::isStarSymbolVisible() const
{
    return endText()->value().toString() == kn::engraving::Pedal::STAR_SYMBOL;
}

void PedalSettingsModel::createProperties()
{
    TextLineSettingsModel::createProperties();

    connect(endText(), &PropertyItem::isModifiedChanged, this, [this]() {
        emit isChangingLineVisibilityAllowedChanged();
    });

    m_lineType = buildPropertyItem(kn::engraving::Pid::END, [this](const kn::engraving::Pid, const QVariant& newValue) {
        setLineType(newValue.toInt());
    }, [this](const kn::engraving::Sid styleId, const QVariant& newValue) {
        updateStyleValue(styleId, newValue);
        emit requestReloadPropertyItems();
    }, [this](const kn::engraving::Pid propertyId) {
        resetPropertyValue(m_elementList, propertyId);
        resetPropertyValue(m_elementList, kn::engraving::Pid::END_HOOK_TYPE);
        resetPropertyValue(m_elementList, kn::engraving::Pid::END_TEXT);
        resetPropertyValue(m_elementList, kn::engraving::Pid::LINE_VISIBLE);
        loadProperties();
    });

    connect(endHookType(), &PropertyItem::isModifiedChanged, m_lineType, &PropertyItem::setIsModified);

    isLineVisible()->setIsVisible(false);
    allowDiagonal()->setIsVisible(false);
    placement()->setIsVisible(false);
}

void PedalSettingsModel::loadProperties()
{
    TextLineSettingsModel::loadProperties();

    m_lineType->setIsEnabled(true);

    if (isStarSymbolVisible()) {
        m_rosetteHookSelected = true;
        m_lineType->updateCurrentValue(int(kn::engraving::HookType::ROSETTE));
    } else {
        m_rosetteHookSelected = false;
        m_lineType->updateCurrentValue(endHookType()->value());
    }
    emit isChangingLineVisibilityAllowedChanged();

    updateStartAndEndHookTypes();
}

void PedalSettingsModel::setLineType(int newType)
{
    m_rosetteHookSelected = (newType == int(kn::engraving::HookType::ROSETTE));
    int hookType = newType;
    QString text = QString();

    if (m_rosetteHookSelected) {
        text = kn::engraving::Pedal::STAR_SYMBOL;
        startHookType()->setValue(static_cast<int>(kn::engraving::HookType::NONE));
    }

    endHookType()->setValue(hookType);
    endText()->setValue(text);
    isLineVisible()->setValue(!m_rosetteHookSelected);

    m_lineType->setValue(newType);
}
