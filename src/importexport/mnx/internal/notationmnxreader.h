/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2026 MuseScore Limited and others
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

#include "project/inotationreader.h"
#include "types/bytearray.h"
#include "modularity/ioc.h"
#include "importexport/mnx/imnxconfiguration.h"

namespace kn::iex::mnxio {
class NotationMnxReader : public project::INotationReader, public kfw::Injectable
{
public:
    NotationMnxReader(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Injectable(iocCtx) {}

    kfw::Ret read(kn::engraving::MasterScore* score, const kfw::io::path_t& path, const Options& options = Options()) override;

private:
    kfw::Ret importJson(kn::engraving::MasterScore* score, kfw::ByteArray&& jsonData, const kfw::io::path_t& path) const;
    kfw::Ret importMnx(kn::engraving::MasterScore* score, kfw::ByteArray&& mnxData, const kfw::io::path_t& path) const;
    const IMnxConfiguration* mnxConfiguration() const;

    kfw::GlobalInject<IMnxConfiguration> m_mnxConfiguration;
};
}
