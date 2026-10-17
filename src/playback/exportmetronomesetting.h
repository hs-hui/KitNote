/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
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

#include <QByteArray>

#include "settings.h"

// KitNote: whether exported audio contains the metronome click.
// The export dialog sets it; the playback controller reads it while audio is exported.
// In MuseScore Studio the click ends up in the file whenever the metronome happens to be switched on.
namespace kn::playback {
inline const kfw::Settings::Key& exportAudioWithMetronomeKey()
{
    // The default also gives the stored value its type: read back from the settings file it is a string.
    static const kfw::Settings::Key key = [] {
        const kfw::Settings::Key k("playback", "export/audioWithMetronome");
        kfw::settings()->setDefaultValue(k, kfw::Val(false));
        return k;
    }();
    return key;
}

inline bool exportAudioWithMetronome()
{
    return kfw::settings()->value(exportAudioWithMetronomeKey()).toBool();
}

inline void setExportAudioWithMetronome(bool withMetronome)
{
    kfw::settings()->setSharedValue(exportAudioWithMetronomeKey(), kfw::Val(withMetronome));
}

// KitNote: whether exported audio contains the score's recording (the Mixer's "Recording" channel). Off
// by default: an exported drum part is the drums. KITNOTE_EXPORT_RECORDING=1 (or 0) decides for a command
// line export (tools/kitnote measures with it that the recording and the score play together).
inline const kfw::Settings::Key& exportAudioWithRecordingKey()
{
    static const kfw::Settings::Key key = [] {
        const kfw::Settings::Key k("playback", "export/audioWithRecording");
        kfw::settings()->setDefaultValue(k, kfw::Val(false));
        return k;
    }();
    return key;
}

inline bool exportAudioWithRecording()
{
    const QByteArray forced = qgetenv("KITNOTE_EXPORT_RECORDING");
    if (!forced.isEmpty()) {
        return forced == "1";
    }
    return kfw::settings()->value(exportAudioWithRecordingKey()).toBool();
}

inline void setExportAudioWithRecording(bool withRecording)
{
    kfw::settings()->setSharedValue(exportAudioWithRecordingKey(), kfw::Val(withRecording));
}
}
