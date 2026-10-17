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

namespace kn::inspector {
class TupletSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable from QML")

    Q_PROPERTY(kn::inspector::PropertyItem * directionType READ directionType CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * numberType READ numberType CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * bracketType READ bracketType CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * lineThickness READ lineThickness CONSTANT)

public:
    explicit TupletSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository);

    PropertyItem* directionType() const;
    PropertyItem* numberType() const;
    PropertyItem* bracketType() const;
    PropertyItem* lineThickness() const;

    Q_INVOKABLE QVariantList possibleNumberTypes() const;
    Q_INVOKABLE QVariantList possibleBracketTypes() const;

private:
    void createProperties() override;
    void loadProperties() override;
    void resetProperties() override;

    PropertyItem* m_directionType = nullptr;
    PropertyItem* m_numberType = nullptr;
    PropertyItem* m_bracketType = nullptr;
    PropertyItem* m_lineThickness = nullptr;
};
}
