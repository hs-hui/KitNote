/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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
#include <optional>

#include <QObject>
#include <QString>

#include "modularity/ioc.h"

#include "global/iglobalconfiguration.h"
#include "iinteractive.h"

#include "audio/main/iplayback.h"
#include "audio/common/audiotypes.h"
#include "midi/miditypes.h"

#include "abstractaudioresourceitem.h"

namespace kn::playback {
class InputResourceItem : public AbstractAudioResourceItem
{
    Q_OBJECT

    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<kfw::audio::IPlayback> playback = { this };

    QML_ELEMENT;
    QML_UNCREATABLE("Must be created in C++ only")

public:
    explicit InputResourceItem(QObject* parent = nullptr);

    void requestAvailableResources() override;
    void handleMenuItem(const QString& menuItemId) override;

    const kfw::audio::AudioInputParams& params() const;
    void setParams(const kfw::audio::AudioInputParams& newParams);
    void setParamsRecourceMeta(const kfw::audio::AudioResourceMeta& newMeta);

    QString title() const override;
    bool isBlank() const override;
    bool isActive() const override;
    bool hasNativeEditorSupport() const override;

signals:
    void inputParamsChanged();
    void inputParamsChangeRequested(const kfw::audio::AudioResourceMeta& newMeta);

private:
    using ResourceByVendorMap = std::map<kfw::audio::AudioResourceVendor, kfw::audio::AudioResourceMetaList>;

    QVariantMap buildMuseMenuItem(const ResourceByVendorMap& resourcesByVendor) const;
    QVariantMap buildVstMenuItem(const ResourceByVendorMap& resourcesByVendor) const;
    QVariantMap buildSoundFontsMenuItem(const ResourceByVendorMap& resourcesByVendor) const;
    QVariantMap buildMsBasicMenuItem(const kfw::audio::AudioResourceMetaList& availableResources, bool isCurrentSoundFont,
                                     const std::optional<kfw::midi::Program>& currentPreset) const;
    QVariantMap buildSoundFontMenuItem(const kfw::String& soundFont, const kfw::audio::AudioResourceMetaList& availableResources,
                                       bool isCurrentSoundFont, const std::optional<kfw::midi::Program>& currentPreset) const;

    void updateAvailableResources(const kfw::audio::AudioResourceMetaList& availableResources);

    std::map<kfw::audio::AudioResourceType, ResourceByVendorMap > m_availableResourceMap;
    kfw::audio::AudioInputParams m_currentInputParams;
};
}
