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
#ifndef KN_IMPORTEXPORT_AUDIOEXPORTCONFIGURATION_H
#define KN_IMPORTEXPORT_AUDIOEXPORTCONFIGURATION_H

#include "iaudioexportconfiguration.h"

namespace kn::iex::audioexport {
class AudioExportConfiguration : public IAudioExportConfiguration
{
public:
    void init();

    int exportMp3Bitrate() const override;
    void setExportMp3Bitrate(int bitrate) override;
    void setExportMp3BitrateOverride(std::optional<int> bitrate) override;
    const std::vector<int>& availableMp3BitRates() const override;

    int exportSampleRate() const override;
    void setExportSampleRate(int rate) override;
    const std::vector<int>& availableSampleRates() const override;

    kfw::audio::samples_t exportBufferSize() const override;

    kfw::audio::AudioSampleFormat exportWavSampleFormat() const override;
    void setExportWavSampleFormat(kfw::audio::AudioSampleFormat format) override;

    kfw::audio::AudioSampleFormat exportFlacSampleFormat() const override;
    void setExportFlacSampleFormat(kfw::audio::AudioSampleFormat format) override;

    const std::vector<kfw::audio::AudioSampleFormat>& availableWavSampleFormats() const override;
    const std::vector<kfw::audio::AudioSampleFormat>& availableFlacSampleFormats() const override;
    QString sampleFormatToString(kfw::audio::AudioSampleFormat format) const override;

private:
    std::optional<int> m_exportMp3BitrateOverride = std::nullopt;
};
}

#endif // KN_IMPORTEXPORT_AUDIOEXPORTCONFIGURATION_H
