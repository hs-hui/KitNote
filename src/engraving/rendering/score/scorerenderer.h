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
#pragma once

#include "../iscorerenderer.h"

namespace kn::engraving {
class Score;
}

namespace kn::engraving::rendering::score  {
class ScoreRenderer : public IScoreRenderer
{
public:

    // Main interface
    void layoutScore(Score* score, const Fraction& st, const Fraction& et) const override;
    void layoutHeadersFooters(Score* score) const override;

    SizeF pageSizeInch(const Score* score) const override;
    SizeF pageSizeInch(const Score* score, const ScorePaintOptions& opt) const override;
    void paintScore(kfw::draw::Painter* painter, Score* score, const ScorePaintOptions& opt) const override;
    void paintItem(kfw::draw::Painter& painter, const EngravingItem* item, const PaintOptions& opt) const override;

    //! TODO Investigation is required, probably these functions or their calls should not be.
    // Other
    void layoutTextLineBaseSegment(TextLineBaseSegment* item) override;
    void layoutBeam1(Beam* item) override;
    void layoutStem(Chord* item) override;

    // Layout Text 1
    void layoutText1(TextBase* item, bool base = false) override;

    void computeBezier(TieSegment* tieSeg, PointF shoulderOffset = PointF()) override;
    void computeBezier(SlurSegment* slurSeg, PointF shoulderOffset = PointF()) override;

    bool scoreHasTimestampHeadersFooters(const Score* score) const override;

private:
    // Layout Single Item
    void doLayoutItem(EngravingItem* item) override;

    void doDrawItem(const EngravingItem* item, kfw::draw::Painter* p, const PaintOptions& opt) override;
};
}
