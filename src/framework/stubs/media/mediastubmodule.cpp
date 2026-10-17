/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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
#include "mediastubmodule.h"

#include "framework/media/ivideoencoder.h"
#include "framework/media/ivideoencoderresolver.h"
#include "framework/media/internal/videoencoderresolver.h"
#include "modularity/ioc.h"

#include "videoencoderstub.h"

using namespace kfw::media;
using namespace kfw::modularity;

std::string MediaModule::moduleName() const
{
    return "media_stub";
}

void MediaModule::registerExports()
{
    auto videoEncoderStub = std::make_shared<VideoEncoderStub>();
    auto videoEncodeResolver = std::make_shared<VideoEncoderResolver>();
    videoEncodeResolver->setCurrentVideoEncoder(videoEncoderStub);

    ioc()->registerExport<IVideoEncoder>(moduleName(), videoEncoderStub);
    ioc()->registerExport<IVideoEncoderResolver>(moduleName(), videoEncodeResolver);
}
