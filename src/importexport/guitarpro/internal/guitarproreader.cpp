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
#include "guitarproreader.h"

#include "io/file.h"

#include "engraving/dom/excerpt.h"
#include "engraving/dom/masterscore.h"
#include "engraving/engravingerrors.h"

namespace kn::iex::guitarpro {
extern kn::engraving::Err importGTP(kn::engraving::MasterScore*, kfw::io::IODevice* io, const kfw::modularity::ContextPtr& iocCtx,
                                    bool experimental = false);

kfw::Ret GuitarProReader::read(kn::engraving::MasterScore* score, const kfw::io::path_t& path, const Options&)
{
    kfw::io::File file(path);
    kn::engraving::Err err = importGTP(score, &file, iocContext(), guitarProConfiguration()->experimental());

    if (guitarProConfiguration()->linkedTabStaffCreated()) {
        engraving::Excerpt::createLinkedTabs(score);
    }

    kfw::io::File styleFile(":/engraving/styles/gp-style.mss");
    score->loadStyle(styleFile);

    return kn::engraving::make_ret(err, path);
}
} // namespace kn::iex::guitarpro
