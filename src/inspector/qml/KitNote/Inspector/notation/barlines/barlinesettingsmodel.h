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
class BarlineSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable from QML")

    Q_PROPERTY(kn::inspector::PropertyItem * type READ type CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * playCount READ playCount CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * playCountText READ playCountText CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * playCountTextSetting READ playCountTextSetting CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * isSpanToNextStaff READ isSpanToNextStaff CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * spanFrom READ spanFrom CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * spanTo READ spanTo CONSTANT)
    Q_PROPERTY(kn::inspector::PropertyItem * hasToShowTips READ hasToShowTips CONSTANT)
    Q_PROPERTY(bool isRepeatStyleChangingAllowed READ isRepeatStyleChangingAllowed NOTIFY isRepeatStyleChangingAllowedChanged)
    Q_PROPERTY(bool showPlayCountSettings READ showPlayCountSettings NOTIFY showPlayCountSettingsChanged FINAL)
    Q_PROPERTY(bool showPlayCount READ showPlayCount NOTIFY showPlayCountChanged FINAL)

public:
    explicit BarlineSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository);

    Q_INVOKABLE void applySpanPreset(const int presetType);
    Q_INVOKABLE void setSpanIntervalAsStaffDefault();

    PropertyItem* type() const;
    PropertyItem* playCount() const;
    PropertyItem* playCountText() const;
    PropertyItem* playCountTextSetting() const;
    PropertyItem* isSpanToNextStaff() const;
    PropertyItem* spanFrom() const;
    PropertyItem* spanTo() const;
    PropertyItem* hasToShowTips() const;

    bool isRepeatStyleChangingAllowed() const;
    bool showPlayCountSettings() const;
    bool showPlayCount() const;

signals:
    void isRepeatStyleChangingAllowedChanged();
    void showPlayCountSettingsChanged(bool show);
    void showPlayCountChanged(bool show);

private:
    void createProperties() override;
    void requestElements() override;
    void loadProperties() override;
    void resetProperties() override;
    void onNotationChanged(const kn::engraving::PropertyIdSet& changedPropertyIdSet,
                           const kn::engraving::StyleIdSet& changedStyleIdSet) override;

    void loadProperties(const kn::engraving::PropertyIdSet& propertyIdSet);

    void updateShowPlayCount();
    void updateShowPlayCountSettings();

    PropertyItem* m_type = nullptr;
    PropertyItem* m_playCount = nullptr;
    PropertyItem* m_playCountText = nullptr;
    PropertyItem* m_playCountTextSetting = nullptr;
    PropertyItem* m_isSpanToNextStaff = nullptr;
    PropertyItem* m_spanFrom = nullptr;
    PropertyItem* m_spanTo = nullptr;
    PropertyItem* m_hasToShowTips = nullptr;

    bool m_showPlayCount = false;
    bool m_showPlayCountSettings = false;
};
}
