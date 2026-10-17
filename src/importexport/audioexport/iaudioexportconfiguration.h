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
#ifndef KN_IMPORTEXPORT_IAUDIOEXPORTCONFIGURATION_H
#define KN_IMPORTEXPORT_IAUDIOEXPORTCONFIGURATION_H

#include <optional>

#include "modularity/imoduleinterface.h"

#include "audio/common/audiotypes.h"

namespace kn::iex::audioexport {
class IAudioExportConfiguration : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IAudioExportConfiguration)

public:
    virtual ~IAudioExportConfiguration() = default;

    virtual int exportMp3Bitrate() const = 0;
    virtual void setExportMp3Bitrate(int bitrate) = 0;
    virtual void setExportMp3BitrateOverride(std::optional<int> bitrate) = 0;
    virtual const std::vector<int>& availableMp3BitRates() const = 0;

    virtual int exportSampleRate() const = 0;
    virtual void setExportSampleRate(int rate) = 0;
    virtual const std::vector<int>& availableSampleRates() const = 0;

    virtual kfw::audio::samples_t exportBufferSize() const = 0;

    virtual kfw::audio::AudioSampleFormat exportWavSampleFormat() const = 0;
    virtual void setExportWavSampleFormat(kfw::audio::AudioSampleFormat format) = 0;

    virtual kfw::audio::AudioSampleFormat exportFlacSampleFormat() const = 0;
    virtual void setExportFlacSampleFormat(kfw::audio::AudioSampleFormat format) = 0;

    virtual const std::vector<kfw::audio::AudioSampleFormat>& availableWavSampleFormats() const = 0;
    virtual const std::vector<kfw::audio::AudioSampleFormat>& availableFlacSampleFormats() const = 0;
    virtual QString sampleFormatToString(kfw::audio::AudioSampleFormat format) const = 0;
};
}

#endif // KN_IMPORTEXPORT_IAUDIOEXPORTCONFIGURATION_H
