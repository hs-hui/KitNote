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

#include "async/channel.h"
#include "engraving/dom/engravingitem.h"
#include "engraving/dom/select.h"

#include <QList>
#include <QObject>
#include <functional>

namespace kn::inspector {
class IElementRepositoryService
{
public:
    virtual ~IElementRepositoryService() = default;

    virtual bool needUpdateElementList(const QList<kn::engraving::EngravingItem*>& newRawElementList,
                                       engraving::SelState selectionState) const = 0;
    virtual void updateElementList(const QList<kn::engraving::EngravingItem*>& newRawElementList, engraving::SelState selectionState) = 0;

    virtual QList<kn::engraving::EngravingItem*> findElementsByType(const kn::engraving::ElementType elementType) const = 0;
    virtual QList<kn::engraving::EngravingItem*> findElementsByType(const kn::engraving::ElementType elementType,
                                                                    std::function<bool(const kn::engraving::EngravingItem*)> filterFunc)
    const = 0;
    virtual QList<kn::engraving::EngravingItem*> takeAllElements() const = 0;

    virtual kfw::async::Channel<QList<kn::engraving::EngravingItem*> > elementsUpdated() const = 0;
};
}
