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

#include <map>

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

#include "modularity/ioc.h"

#include "global/iglobalconfiguration.h"
#include "iinteractive.h"

#include "audio/main/iplayback.h"
#include "audio/common/audiotypes.h"

#include "abstractaudioresourceitem.h"

#if (defined(_MSCVER) || defined(_MSC_VER))
// unreferenced function with internal linkage has been removed
#pragma warning(disable: 4505)
#endif

namespace kn::playback {
class OutputResourceItem : public AbstractAudioResourceItem
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id NOTIFY fxParamsChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE setIsActive NOTIFY isActiveChanged)

    QML_ELEMENT;
    QML_UNCREATABLE("Must be created in C++ only")

    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<kfw::audio::IPlayback> playback = { this };

public:
    explicit OutputResourceItem(QObject* parent, const kfw::audio::AudioFxParams& params);

    void requestAvailableResources() override;
    void handleMenuItem(const QString& menuItemId) override;

    const kfw::audio::AudioFxParams& params() const;
    void setParams(const kfw::audio::AudioFxParams& params);

    QString title() const override;
    bool isBlank() const override;
    bool isActive() const override;
    bool hasNativeEditorSupport() const override;

    QString id() const;

public slots:
    void setIsActive(bool newIsActive);

signals:
    void fxParamsChanged();

private:
    void updateCurrentFxParams(const kfw::audio::AudioResourceMeta& newMeta);
    void updateAvailableFxVendorsMap(const kfw::audio::AudioResourceMetaList& availableFxResources);

    std::map<kfw::audio::AudioResourceVendor, kfw::audio::AudioResourceMetaList> m_fxByVendorMap;

    kfw::audio::AudioFxParams m_currentFxParams;
};
}
