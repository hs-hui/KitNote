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
#pragma once

#include <qqmlintegration.h>

#include "abstractinspectormodel.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"

namespace kn::inspector {
class InstrumentNameSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable from QML")

    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };

public:
    explicit InstrumentNameSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                         IElementRepositoryService* repository);

    Q_INVOKABLE void openStyleSettings();
    Q_INVOKABLE void openStaffAndPartProperties();

private:
    const kn::engraving::InstrumentName* selectedInstrumentName() const;

    void createProperties() override {}
    void loadProperties() override {}
    void resetProperties() override {}
};
}
