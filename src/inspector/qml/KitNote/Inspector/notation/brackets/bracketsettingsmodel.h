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
class BracketSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable from QML")

    Q_PROPERTY(kn::inspector::PropertyItem * bracketColumnPosition READ bracketColumnPosition CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * bracketSpanStaves READ bracketSpanStaves CONSTANT)

    Q_PROPERTY(bool areSettingsAvailable READ areSettingsAvailable NOTIFY selectionChanged)
    Q_PROPERTY(int maxBracketColumnPosition READ maxBracketColumnPosition NOTIFY maxBracketColumnPositionChanged)
    Q_PROPERTY(int maxBracketSpanStaves READ maxBracketSpanStaves NOTIFY selectionChanged)

public:

    explicit BracketSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository);

    void createProperties() override;
    void requestElements() override;
    void loadProperties() override;
    void resetProperties() override;

    PropertyItem* bracketColumnPosition() const;
    PropertyItem* bracketSpanStaves() const;

    bool areSettingsAvailable() const;
    int maxBracketColumnPosition() const;
    int maxBracketSpanStaves() const;

signals:
    void selectionChanged();
    void maxBracketColumnPositionChanged();

private:
    void onNotationChanged(const kn::engraving::PropertyIdSet& changedPropertyIdSet,
                           const kn::engraving::StyleIdSet& changedStyleIdSet) override;

    void loadProperties(const kn::engraving::PropertyIdSet& propertyIdSet);

    PropertyItem* m_bracketColumnPosition = nullptr;
    PropertyItem* m_bracketSpanStaves = nullptr;
};
}
