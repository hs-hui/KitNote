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

#include "modularity/imoduleinterface.h"

#include "convertertypes.h"

#include "global/types/ret.h"
#include "global/types/uri.h"
#include "global/io/path.h"
#include "global/progress.h"

namespace kn::converter {
class IConverterController : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IConverterController)
public:
    virtual ~IConverterController() = default;

    virtual kfw::Ret fileConvert(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {},
                                  const kfw::String& soundProfile = {}, const kfw::io::path_t& tracksDiffPath = {},
                                  const std::string& transposeOptionsJson = {},
                                  const std::optional<ConvertTarget>& target = std::nullopt) = 0;

    virtual kfw::Ret batchConvert(const kfw::io::path_t& batchJobFile, const OpenParams& openParams = {},
                                   const kfw::String& soundProfile = {}, kfw::ProgressPtr progress = nullptr) = 0;

    virtual kfw::Ret convertScoreParts(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {}) = 0;

    virtual kfw::Ret exportScoreMedia(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {},
                                       const kfw::io::path_t& highlightConfigPath = kfw::io::path_t()) = 0;
    virtual kfw::Ret exportScoreMeta(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {}) = 0;
    virtual kfw::Ret exportScoreParts(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {}) = 0;
    virtual kfw::Ret exportScorePartsPdfs(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {}) = 0;
    virtual kfw::Ret exportScoreTranspose(const kfw::io::path_t& in, const kfw::io::path_t& out, const std::string& optionsJson,
                                           const OpenParams& openParams = {}) = 0;

    virtual kfw::Ret exportScoreElements(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams = {}) = 0;

    virtual kfw::Ret updateSource(const kfw::io::path_t& in, const std::string& newSource, bool forceMode = false) = 0;
};
}
