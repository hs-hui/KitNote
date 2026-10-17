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

#include "flacwriter.h"

#include "log.h"

using namespace kfw::audio;
using namespace kn::iex::audioexport;
using namespace kfw::io;

kfw::Ret FlacWriter::write(notation::INotationPtr notation, kfw::io::IODevice& destinationDevice, const Options& options)
{
    const SoundTrackFormat format {
        SoundTrackType::FLAC,
        {
            static_cast<sample_rate_t>(configuration()->exportSampleRate()),
            configuration()->exportBufferSize(),
            2 /* audioChannelsNumber */
        },
        configuration()->exportFlacSampleFormat(),
        0 /* bitRate */
    };

    return doWriteAndWait(notation, destinationDevice, format, options);
}
