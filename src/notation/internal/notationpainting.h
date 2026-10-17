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
#ifndef KN_NOTATION_NOTATIONPAINTING_H
#define KN_NOTATION_NOTATIONPAINTING_H

#include "../inotationpainting.h"

#include "modularity/ioc.h"
#include "../inotationconfiguration.h"
#include "engraving/iengravingconfiguration.h"
#include "engraving/rendering/iscorerenderer.h"
#include "ui/iuiconfiguration.h"

namespace kn::engraving {
class Score;
class Page;
}

namespace kn::notation {
class Notation;
class NotationPainting : public INotationPainting, public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> configuration;
    kfw::GlobalInject<engraving::IEngravingConfiguration> engravingConfiguration;
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::ContextInject<engraving::rendering::IScoreRenderer> scoreRenderer = { this };

public:
    NotationPainting(Notation* notation, const kfw::modularity::ContextPtr& ctx);

    void setViewMode(const ViewMode& viewMode) override;
    ViewMode viewMode() const override;
    kfw::async::Notification viewModeChanged() const override;

    int pageCount() const override;
    kfw::SizeF pageSizeInch() const override;
    kfw::SizeF pageSizeInch(const Options& opt) const override;

    void paintView(kfw::draw::Painter* painter, const kfw::RectF& frameRect, bool isPrinting) override;
    void paintPdf(kfw::draw::Painter* painter, const Options& opt) override;
    void paintPrint(kfw::draw::Painter* painter, const Options& opt) override;
    void paintPng(kfw::draw::Painter* painter, const Options& opt) override;

private:
    kn::engraving::Score* score() const;

    bool isPaintPageBorder() const;
    void doPaint(kfw::draw::Painter* painter, const Options& opt);
    void paintPageBorder(kfw::draw::Painter* painter, const kn::engraving::Page* page) const;
    void paintPageSheet(kfw::draw::Painter* painter, const engraving::Page* page, const kfw::RectF& pageRect,
                        bool printPageBackground) const;

    Notation* m_notation = nullptr;

    kfw::async::Notification m_viewModeChanged;
};
}

#endif // KN_NOTATION_NOTATIONPAINTING_H
