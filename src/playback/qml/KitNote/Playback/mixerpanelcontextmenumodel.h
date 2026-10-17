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

#include "actions/actionable.h"
#include "uicomponents/qml/KitNote/UiComponents/abstractmenumodel.h"

#include "actions/iactionsdispatcher.h"
#include "playback/iplaybackconfiguration.h"

#include "playback/playbacktypes.h"

namespace kn::playback {
class MixerPanelContextMenuModel : public kfw::uicomponents::AbstractMenuModel, public kfw::actions::Actionable
{
    Q_OBJECT

    Q_PROPERTY(bool labelsSectionVisible READ labelsSectionVisible NOTIFY labelsSectionVisibleChanged)
    Q_PROPERTY(bool soundSectionVisible READ soundSectionVisible NOTIFY soundSectionVisibleChanged)
    Q_PROPERTY(bool audioFxSectionVisible READ audioFxSectionVisible NOTIFY audioFxSectionVisibleChanged)
    Q_PROPERTY(bool auxSendsSectionVisible READ auxSendsSectionVisible NOTIFY auxSendsSectionVisibleChanged)
    Q_PROPERTY(bool balanceSectionVisible READ balanceSectionVisible NOTIFY balanceSectionVisibleChanged)
    Q_PROPERTY(bool volumeSectionVisible READ volumeSectionVisible NOTIFY volumeSectionVisibleChanged)
    Q_PROPERTY(bool faderSectionVisible READ faderSectionVisible NOTIFY faderSectionVisibleChanged)
    Q_PROPERTY(bool muteAndSoloSectionVisible READ muteAndSoloSectionVisible NOTIFY muteAndSoloSectionVisibleChanged)
    Q_PROPERTY(bool titleSectionVisible READ titleSectionVisible NOTIFY titleSectionVisibleChanged)

    QML_ELEMENT

    kfw::GlobalInject<playback::IPlaybackConfiguration> configuration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };

public:
    explicit MixerPanelContextMenuModel(QObject* parent = nullptr);

    bool labelsSectionVisible() const;
    bool soundSectionVisible() const;
    bool audioFxSectionVisible() const;
    bool auxSendsSectionVisible() const;
    bool balanceSectionVisible() const;
    bool volumeSectionVisible() const;
    bool faderSectionVisible() const;
    bool muteAndSoloSectionVisible() const;
    bool titleSectionVisible() const;

    Q_INVOKABLE void load() override;

signals:
    void labelsSectionVisibleChanged();
    void soundSectionVisibleChanged();
    void audioFxSectionVisibleChanged();
    void auxSendsSectionVisibleChanged();
    void balanceSectionVisibleChanged();
    void volumeSectionVisibleChanged();
    void faderSectionVisibleChanged();
    void muteAndSoloSectionVisibleChanged();
    void titleSectionVisibleChanged();

private:
    bool isSectionVisible(MixerSectionType sectionType) const;

    kfw::uicomponents::MenuItem* buildSectionVisibleItem(MixerSectionType sectionType);
    kfw::uicomponents::MenuItem* buildAuxSendVisibleItem(kfw::audio::aux_channel_idx_t index);
    kfw::uicomponents::MenuItem* buildAuxChannelVisibleItem(kfw::audio::aux_channel_idx_t index);

    void toggleMixerSection(const kfw::actions::ActionData& args);
    void toggleAuxSend(const kfw::actions::ActionData& args);
    void toggleAuxChannel(const kfw::actions::ActionData& args);

    void emitMixerSectionVisibilityChanged(MixerSectionType sectionType);

    void setViewMenuItemChecked(const QString& itemId, bool checked);
};
}
