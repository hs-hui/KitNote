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
#include "writescorehook.h"

#include "rw/xmlwriter.h"

#include "dom/masterscore.h"
#include "dom/excerpt.h"

#include "rw/write/writer.h"

using namespace kn::engraving;
using namespace kn::engraving::compat;
using namespace kn::engraving::write;

void WriteScoreHook::onWriteStyle302(Score* score, XmlWriter& xml)
{
    bool isWriteStyle = false;

    //! NOTE If not the master score, because the Excerpts (parts) have not yet been write to separate files
    if (!score->isMaster()) {
        isWriteStyle = true;
    }

    //! NOTE If the test mode, because the tests have not yet been adapted to the new format
    if (EngravingGlobals::testMode && EngravingGlobals::testWriteStyleToScore) {
        isWriteStyle = true;
    }

    if (isWriteStyle) {
        score->style().save(xml, true);     // save only differences to builtin style (logic from 3.)
    }
}

void WriteScoreHook::onWriteExcerpts302(Score* score, XmlWriter& xml, WriteContext& ctx)
{
    bool isWriteExcerpts = false;

    //! NOTE If the test mode, because the tests have not yet been adapted to the new format
    if (EngravingGlobals::testMode) {
        isWriteExcerpts = true;
    }

    if (isWriteExcerpts && score->isMaster() && !ctx.shouldWriteRange()) {
        MasterScore* mScore = static_cast<MasterScore*>(score);
        for (const Excerpt* excerpt : mScore->excerpts()) {
            if (excerpt->excerptScore() != score) {
                write::Writer::write(excerpt->excerptScore(), xml, ctx, *this); // recursion write
            }
        }
    }
}
