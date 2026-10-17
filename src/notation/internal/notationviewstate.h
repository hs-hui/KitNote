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

#include "../inotationviewstate.h"
#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "../inotationconfiguration.h"

namespace kn::notation {
class Notation;
class NotationViewState : public INotationViewState, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> configuration;

public:
    explicit NotationViewState(Notation* notation, const kfw::modularity::ContextPtr& ctx);

    kfw::Ret read(const engraving::MscReader& reader, const kfw::io::path_t& pathPrefix = "") override;
    kfw::Ret write(engraving::MscWriter& writer, const kfw::io::path_t& pathPrefix = "") override;

    bool isMatrixInited() const override;
    void setMatrixInited(bool inited) override;

    const kfw::draw::Transform& matrix() const override;
    kfw::async::Channel<kfw::draw::Transform, NotationPaintView*> matrixChanged() const override;
    void setMatrix(const kfw::draw::Transform& matrix, NotationPaintView* sender) override;

    kfw::ValCh<int> zoomPercentage() const override;

    kfw::ValCh<ZoomType> zoomType() const override;
    void setZoomType(ZoomType type) override;

    ViewMode viewMode() const override;
    void setViewMode(const ViewMode& mode) override;

    kfw::async::Notification stateChanged() const override;

    void makeDefault() override;

private:
    bool m_isMatrixInited = false;
    kfw::draw::Transform m_matrix;
    kfw::async::Channel<kfw::draw::Transform, NotationPaintView*> m_matrixChanged;
    kfw::ValCh<int> m_zoomPercentage;
    kfw::ValCh<ZoomType> m_zoomType;

    notation::ViewMode m_viewMode = notation::ViewMode::PAGE;

    kfw::async::Notification m_stateChanged;
};
}
