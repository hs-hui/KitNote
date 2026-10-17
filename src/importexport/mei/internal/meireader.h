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

#include "project/inotationreader.h"

#include "modularity/ioc.h"
#include "iinteractive.h"
#include "io/ifilesystem.h"

#include "engraving/engravingerrors.h"

namespace kn::iex::mei {
class MeiReader : public project::INotationReader, public kfw::Contextable
{
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;

public:

    MeiReader(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    kfw::Ret read(kn::engraving::MasterScore* score, const kfw::io::path_t& path, const Options& options = Options()) override;
    kn::engraving::Err import(kn::engraving::MasterScore* score, const kfw::io::path_t& path, const Options& options = Options());

private:
    bool askToLoadDespiteWarnings(const kfw::String& text, const kfw::String& detailedText);
};
}
