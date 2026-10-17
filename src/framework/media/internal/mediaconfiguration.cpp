/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
 * Modified for KitNote, 2026
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#include "mediaconfiguration.h"

#include "settings.h"

using namespace kfw::media;

static const kfw::Settings::Key FFMPEG_LIBS_DIR("media", "application/ffmpegLibsDir");

void MediaConfiguration::init()
{
    settings()->setDefaultValue(FFMPEG_LIBS_DIR, Val(""));
    settings()->valueChanged(FFMPEG_LIBS_DIR).onReceive(this, [this](const Val& val) {
        m_ffmpegLibsDirChanged.send(val.toPath());
    });
}

kfw::io::path_t MediaConfiguration::ffmpegLibsDir() const
{
    return settings()->value(FFMPEG_LIBS_DIR).toPath();
}

void MediaConfiguration::setFFmpegLibsDir(const io::path_t& path)
{
    settings()->setSharedValue(FFMPEG_LIBS_DIR, Val(path));
}

kfw::async::Channel<kfw::io::path_t> MediaConfiguration::ffmpegLibsDirChanged() const
{
    return m_ffmpegLibsDirChanged;
}
