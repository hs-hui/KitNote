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

#include "abstractinspectorproxymodel.h"

namespace kn::inspector {
class NoteSettingsProxyModel : public AbstractInspectorProxyModel
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable from QML")

public:
    explicit NoteSettingsProxyModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository);

private:
    void onElementsUpdated(const QList<kn::engraving::EngravingItem*>& newElements);

    InspectorModelType resolveDefaultSubModelType(const QList<kn::engraving::EngravingItem*>& newElements) const;
};
}
