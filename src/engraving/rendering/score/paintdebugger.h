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
#ifndef KN_ENGRAVING_PAINTDEBUGGER_DEV_H
#define KN_ENGRAVING_PAINTDEBUGGER_DEV_H

#include "draw/ipaintprovider.h"

namespace kn::engraving::rendering::score {
class PaintDebugger : public kfw::draw::IPaintProvider
{
public:
    PaintDebugger(kfw::draw::IPaintProviderPtr real);

    kfw::draw::IPaintProviderPtr realProvider() const;
    void setDebugPenColor(const kfw::draw::Color& c);
    void restorePenColor();

    bool isActive() const override;
    void beginTarget(const std::string& name) override;
    void beforeEndTargetHook(kfw::draw::Painter* painter) override;
    bool endTarget(bool endDraw = false) override;

    void beginObject(const std::string& name) override;
    void endObject() override;

    void setAntialiasing(bool arg) override;
    void setCompositionMode(kfw::draw::CompositionMode mode) override;
    void setWindow(const kfw::RectF& window) override;
    void setViewport(const kfw::RectF& viewport) override;

    void setFont(const kfw::draw::Font& font) override;
    const kfw::draw::Font& font() const override;

    void setPen(const kfw::draw::Pen& pen) override;
    void setNoPen() override;
    const kfw::draw::Pen& pen() const override;

    void setBrush(const kfw::draw::Brush& brush) override;
    const kfw::draw::Brush& brush() const override;

    void save() override;
    void restore() override;

    double deviceLogicalDpi() const override;

    void setTransform(const kfw::draw::Transform& transform) override;
    const kfw::draw::Transform& transform() const override;

    void drawPath(const kfw::draw::PainterPath& path) override;
    void drawPolygon(const kfw::PointF* points, size_t pointCount, kfw::draw::PolygonMode mode) override;

    void drawText(const kfw::PointF& point, const kfw::String& text) override;
    void drawText(const kfw::RectF& rect, int flags, const kfw::String& text) override;

    void drawSymbol(const kfw::PointF& point, char32_t ucs4Code) override;

    void drawPixmap(const kfw::PointF& p, const kfw::draw::Pixmap& pm) override;
    void drawTiledPixmap(const kfw::RectF& rect, const kfw::draw::Pixmap& pm, const kfw::PointF& offset = kfw::PointF()) override;

#ifndef NO_QT_SUPPORT
    void drawPixmap(const kfw::PointF& point, const QPixmap& pm) override;
    void drawTiledPixmap(const kfw::RectF& rect, const QPixmap& pm, const kfw::PointF& offset = kfw::PointF()) override;
#endif

    bool hasClipping() const override;

    void setClipRect(const kfw::RectF& rect) override;
    void setMask(const kfw::RectF& background, const std::vector<kfw::RectF>& maskRects) override;
    void setClipping(bool enable) override;

private:
    kfw::draw::IPaintProviderPtr m_real = nullptr;

    kfw::draw::Color m_originalPenColor;
    kfw::draw::Color m_debugPenColor;
};
}

#endif // KN_ENGRAVING_PAINTDEBUGGER_DEV_H
