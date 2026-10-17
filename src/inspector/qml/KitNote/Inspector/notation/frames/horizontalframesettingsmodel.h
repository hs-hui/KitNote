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
class HorizontalFrameSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable from QML")

    Q_PROPERTY(kn::inspector::PropertyItem * frameWidth READ frameWidth CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * leftGap READ leftGap CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * rightGap READ rightGap CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * shouldDisplayKeysAndBrackets READ shouldDisplayKeysAndBrackets CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * isSizeSpatiumDependent READ isSizeSpatiumDependent CONSTANT)

public:
    explicit HorizontalFrameSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                          IElementRepositoryService* repository);

    PropertyItem* frameWidth() const;
    PropertyItem* leftGap() const;
    PropertyItem* rightGap() const;
    PropertyItem* shouldDisplayKeysAndBrackets() const;
    PropertyItem* isSizeSpatiumDependent() const;

private:
    void createProperties() override;
    void requestElements() override;
    void loadProperties() override;
    void resetProperties() override;
    void onNotationChanged(const kn::engraving::PropertyIdSet& changedPropertyIdSet,
                           const kn::engraving::StyleIdSet& changedStyleIdSet) override;

    void loadProperties(const kn::engraving::PropertyIdSet& propertyIdSet);

    PropertyItem* m_frameWidth = nullptr;
    PropertyItem* m_leftGap = nullptr;
    PropertyItem* m_rightGap = nullptr;
    PropertyItem* m_shouldDisplayKeysAndBrackets = nullptr;
    PropertyItem* m_isSizeSpatiumDependent = nullptr;
};
}
