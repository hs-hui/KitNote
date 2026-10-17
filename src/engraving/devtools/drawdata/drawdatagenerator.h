/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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
#ifndef KN_ENGRAVING_DRAWDATAGENERATOR_H
#define KN_ENGRAVING_DRAWDATAGENERATOR_H

#include "global/types/ret.h"
#include "global/io/path.h"
#include "draw/types/drawdata.h"

#include "modularity/ioc.h"
#include "engraving/rendering/iscorerenderer.h"

#include "drawdatatypes.h"

namespace kn::engraving {
class MasterScore;
class DrawDataGenerator : public kfw::Contextable
{
    kfw::ContextInject<engraving::rendering::IScoreRenderer> scoreRenderer = { this };
public:
    DrawDataGenerator(const kfw::modularity::ContextPtr& iocCtx);

    kfw::Ret processDir(const kfw::io::path_t& scoreDir, const kfw::io::path_t& outDir, const GenOpt& opt = GenOpt());
    kfw::Ret processFile(const kfw::io::path_t& scoreFile, const kfw::io::path_t& outFile, const GenOpt& opt = GenOpt());

    kfw::draw::DrawDataPtr genDrawData(const kfw::io::path_t& scorePath, const GenOpt& opt = GenOpt()) const;
    kfw::draw::Pixmap genImage(const kfw::io::path_t& scorePath) const;

private:
    bool loadScore(engraving::MasterScore* score, const kfw::io::path_t& path) const;
    void applyOptions(engraving::MasterScore* score, const GenOpt& opt) const;
};
}

#endif // KN_ENGRAVING_DRAWDATAGENERATOR_H
