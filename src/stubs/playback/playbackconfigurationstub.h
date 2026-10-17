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
#ifndef KN_PLAYBACK_PLAYBACKCONFIGURATIONSTUB_H
#define KN_PLAYBACK_PLAYBACKCONFIGURATIONSTUB_H

#include "playback/iplaybackconfiguration.h"

namespace kn::playback {
class PlaybackConfigurationStub : public IPlaybackConfiguration
{
public:
    bool playNotesWhenEditing() const override;
    void setPlayNotesWhenEditing(bool value) override;
    kfw::async::Notification playNotesWhenEditingChanged() const override;

    bool playChordWhenEditing() const override;
    void setPlayChordWhenEditing(bool value) override;
    kfw::async::Channel<bool> playChordWhenEditingChanged() const override;

    bool playHarmonyWhenEditing() const override;
    void setPlayHarmonyWhenEditing(bool value) override;
    kfw::async::Channel<bool> playHarmonyWhenEditingChanged() const override;

    bool playNotesOnMidiInput() const override;
    void setPlayNotesOnMidiInput(bool value) override;
    kfw::async::Channel<bool> playNotesOnMidiInputChanged() const override;

    PlaybackCursorType cursorType() const override;

    bool isMixerSectionVisible(MixerSectionType sectionType) const override;
    void setMixerSectionVisible(MixerSectionType sectionType, bool visible) override;
    kfw::async::Channel<MixerSectionType, bool> isMixerSectionVisibleChanged() const override;

    bool isAuxSendVisible(kfw::audio::aux_channel_idx_t index) const override;
    void setAuxSendVisible(kfw::audio::aux_channel_idx_t index, bool visible) override;
    kfw::async::Channel<kfw::audio::aux_channel_idx_t, bool> isAuxSendVisibleChanged() const override;

    bool isAuxChannelVisible(kfw::audio::aux_channel_idx_t index) const override;
    void setAuxChannelVisible(kfw::audio::aux_channel_idx_t index, bool visible) const override;
    kfw::async::Channel<kfw::audio::aux_channel_idx_t, bool> isAuxChannelVisibleChanged() const override;

    kfw::audio::gain_t defaultAuxSendValue(kfw::audio::aux_channel_idx_t index, kfw::audio::AudioSourceType sourceType,
                                            const kfw::String& instrumentSoundId) const override;

    bool muteHiddenInstruments() const override;
    void setMuteHiddenInstruments(bool mute) override;
    kfw::async::Channel<bool> muteHiddenInstrumentsChanged() const override;

    const SoundProfileName& basicSoundProfileName() const override;
    const SoundProfileName& museSoundsProfileName() const override;
    const SoundProfileName& compatMuseSoundsProfileName() const override;

    SoundProfileName defaultProfileForNewProjects() const override;
    void setDefaultProfileForNewProjects(const SoundProfileName& name) override;

    bool soundPresetsMultiSelectionEnabled() const override;
    void setSoundPresetsMultiSelectionEnabled(bool enabled) override;

    bool needToShowResetSoundFlagsWhenChangeSoundWarning() const override;
    void setNeedToShowResetSoundFlagsWhenChangeSoundWarning(bool show) override;

    bool needToShowResetSoundFlagsWhenChangePlaybackProfileWarning() const override;
    void setNeedToShowResetSoundFlagsWhenChangePlaybackProfileWarning(bool show) override;

    bool shouldShowOnlineSoundsProcessingError() const override;
    void setShouldShowOnlineSoundsProcessingError(bool show) override;
    kfw::async::Notification shouldShowOnlineSoundsProcessingErrorChanged() const override;

    kfw::String onlineSoundsHandbookUrl() const override;

    OnlineSoundsShowProgressBarMode onlineSoundsShowProgressBarMode() const override;
    void setOnlineSoundsShowProgressBarMode(OnlineSoundsShowProgressBarMode mode) override;
    kfw::async::Notification onlineSoundsShowProgressBarModeChanged() const override;

    bool shouldMeasureInputLag() const override;
};
}

#endif // KN_PLAYBACK_PLAYBACKCONFIGURATIONSTUB_H
