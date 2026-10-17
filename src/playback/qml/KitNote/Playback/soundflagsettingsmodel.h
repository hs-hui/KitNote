/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include <QObject>
#include <qqmlintegration.h>

#include "modularity/ioc.h"
#include "playback/iplaybackcontroller.h"
#include "playback/iplaybackconfiguration.h"

#include "uicomponents/qml/KitNote/UiComponents/menuitem.h"

#include "notationscene/qml/KitNote/NotationScene/abstractelementpopupmodel.h"

namespace kn::playback {
class SoundFlagSettingsModel : public kn::notation::AbstractElementPopupModel
{
    Q_OBJECT

    Q_PROPERTY(bool inited READ inited NOTIFY initedChanged FINAL)

    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged FINAL)

    Q_PROPERTY(QRect iconRect READ iconRect NOTIFY iconRectChanged FINAL)

    Q_PROPERTY(QVariantList availablePresets READ availablePresets NOTIFY availablePresetsChanged FINAL)
    Q_PROPERTY(QStringList selectedPresetCodes READ selectedPresetCodes NOTIFY selectedPresetCodesChanged FINAL)

    Q_PROPERTY(QVariantList availablePlayingTechniques READ availablePlayingTechniques NOTIFY availablePlayingTechniquesChanged FINAL)
    Q_PROPERTY(QString selectedPlayingTechniqueCode READ selectedPlayingTechniqueCode NOTIFY selectedPlayingTechniqueCodeChanged FINAL)

    Q_PROPERTY(QVariantList contextMenuModel READ contextMenuModel NOTIFY contextMenuModelChanged FINAL)

    QML_ELEMENT

    kfw::GlobalInject<IPlaybackConfiguration> playbackConfiguration;
    kfw::ContextInject<IPlaybackController> playbackController = { this };

public:
    explicit SoundFlagSettingsModel(QObject* parent = nullptr);

    bool inited() const;

    Q_INVOKABLE void init() override;
    Q_INVOKABLE void togglePreset(const QString& presetCode);
    Q_INVOKABLE void togglePlayingTechnique(const QString& playingTechniqueCode);

    QVariantList contextMenuModel();
    Q_INVOKABLE void handleContextMenuItem(const QString& menuId);

    QString title() const;
    void setTitle(const QString& title);

    QRect iconRect() const;

    QVariantList availablePresets() const;
    QStringList selectedPresetCodes() const;

    QVariantList availablePlayingTechniques() const;
    QString selectedPlayingTechniqueCode() const;

signals:
    void initedChanged();

    void iconRectChanged(const QRect& rect);

    void titleChanged();

    void availablePresetsChanged();
    void selectedPresetCodesChanged();

    void availablePlayingTechniquesChanged();
    void selectedPlayingTechniqueCodeChanged();

    void contextMenuModelChanged();

private:
    void load();

    project::IProjectAudioSettingsPtr audioSettings() const;
    const kfw::audio::AudioInputParams& currentAudioInputParams() const;

    void initTitle();
    void initAvailablePresets();
    void initAvailablePlayingTechniques();

    void setAvailableSoundPresets(const kfw::audio::SoundPresetList& presets);
    void loadAvailablePlayingTechniques();

    kfw::uicomponents::MenuItem* buildMenuItem(const QString& actionCode, const kfw::TranslatableString& title, bool enabled = true);

    QString defaultPresetCode() const;
    QString defaultPlayingTechniqueCode() const;

    bool updateStaffText();

    QString m_title;

    kfw::audio::SoundPresetList m_availablePresets;

    QVariantList m_availablePresetsModel;
    QVariantList m_availablePlayingTechniquesModel;

    bool m_availablePresetsInited = false;
};
}
