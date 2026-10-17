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

#include <vector>

#include "modularity/ioc.h"
#include "notation/inotationconfiguration.h"
#include "engraving/iengravingconfiguration.h"
#include "engraving/rendering/iscorerenderer.h"

#include "draw/types/geometry.h"
#include "notation/inotation.h"

namespace kn::engraving {
class Score;
class Text;
class Clef;
class KeySig;
class TimeSig;
class BarLine;
class Segment;
}

namespace kfw::draw {
class Painter;
}

namespace kn::notation {
class ContinuousPanel : public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<engraving::IEngravingConfiguration> engravingConfiguration;
    kfw::ContextInject<engraving::rendering::IScoreRenderer> scoreRender = { this };

public:
    ContinuousPanel(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}
    ~ContinuousPanel();

    void setNotation(INotationPtr notation);

    struct NotationViewContext {
        qreal xOffset = 0.;
        qreal yOffset = 0.;
        qreal scaling = 0.;
        std::function<kfw::PointF(const kfw::PointF&)> fromLogical;
    };

    qreal width() const;
    void paint(kfw::draw::Painter& painter, const NotationViewContext& ctx, const engraving::rendering::PaintOptions& opt);

private:
    void clearCache();
    void ensureCacheSize(size_t staffCount);

    INotationPtr m_notation;
    qreal m_width = 0;
    kfw::RectF m_rect;

    kn::engraving::Text* m_cachedMeasureNumberText = nullptr;
    std::vector<kn::engraving::Text*> m_cachedStaffNameTexts;
    std::vector<kn::engraving::Clef*> m_cachedClefs;
    std::vector<kn::engraving::KeySig*> m_cachedKeySigs;
    std::vector<kn::engraving::TimeSig*> m_cachedTimeSigs;
    std::vector<kn::engraving::BarLine*> m_cachedBarLines;
};
}
