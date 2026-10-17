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
class ImageSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable from QML")

    Q_PROPERTY(kn::inspector::PropertyItem * shouldScaleToFrameSize READ shouldScaleToFrameSize CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * height READ height CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * width READ width CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * isAspectRatioLocked READ isAspectRatioLocked CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * isSizeInSpatiums READ isSizeInSpatiums CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * isImageFramed READ isImageFramed CONSTANT)

public:
    explicit ImageSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository);

    PropertyItem* shouldScaleToFrameSize() const;
    PropertyItem* height() const;
    PropertyItem* width() const;
    PropertyItem* isAspectRatioLocked() const;
    PropertyItem* isSizeInSpatiums() const;
    PropertyItem* isImageFramed() const;

private:
    void createProperties() override;
    void requestElements() override;
    void loadProperties() override;
    void resetProperties() override;

    void onNotationChanged(const kn::engraving::PropertyIdSet& changedPropertyIdSet,
                           const kn::engraving::StyleIdSet& changedStyleIdSet) override;

    void loadProperties(const kn::engraving::PropertyIdSet& propertyIdSet);
    void updateFrameScalingAvailability();

    PropertyItem* m_shouldScaleToFrameSize = nullptr;
    PropertyItem* m_height = nullptr;
    PropertyItem* m_width = nullptr;
    PropertyItem* m_isAspectRatioLocked = nullptr;
    PropertyItem* m_isSizeInSpatiums = nullptr;
    PropertyItem* m_isImageFramed = nullptr;
};
}
