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

#include "voltasettingsmodel.h"

#include "translation.h"

#include "ui/view/iconcodes.h"

using namespace kn::inspector;

using IconCode = kfw::ui::IconCode::Code;

VoltaSettingsModel::VoltaSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : TextLineSettingsModel(parent, iocCtx, repository, kn::engraving::ElementType::VOLTA)
{
    setModelType(InspectorModelType::TYPE_VOLTA);
    setTitle(kfw::qtrc("inspector", "Volta"));
    setIcon(kfw::ui::IconCode::Code::VOLTA);

    createProperties();
}

PropertyItem* VoltaSettingsModel::repeatCount() const
{
    return m_repeatCount;
}

void VoltaSettingsModel::updateStartAndEndHookTypes()
{
    setPossibleStartHookTypes({});

    static const QList<HookTypeInfo> endHookTypes {
        { kn::engraving::HookType::NONE, IconCode::LINE_WITH_INVERTED_START_HOOK, kfw::qtrc("inspector", "Normal", "volta hook type") },
        { kn::engraving::HookType::HOOK_90, IconCode::LINE_WITH_TWO_INVERTED_HOOKS,
          kfw::qtrc("inspector", "Hooked 90°", "volta hook type") }
    };

    setPossibleEndHookTypes(endHookTypes);
}

void VoltaSettingsModel::createProperties()
{
    TextLineSettingsModel::createProperties();

    m_repeatCount = buildPropertyItem(kn::engraving::Pid::VOLTA_ENDING);

    isLineVisible()->setIsVisible(true);
    allowDiagonal()->setIsVisible(true);
    placement()->setIsVisible(false);
}

void VoltaSettingsModel::loadProperties()
{
    TextLineSettingsModel::loadProperties();

    loadPropertyItem(m_repeatCount);
    updateStartAndEndHookTypes();
}

void VoltaSettingsModel::resetProperties()
{
    TextLineSettingsModel::resetProperties();

    m_repeatCount->resetToDefault();
}
