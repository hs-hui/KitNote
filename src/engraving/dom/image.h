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

#include "bsymbol.h"

#include "modularity/ioc.h"
#include "draw/iimageprovider.h"

namespace kfw::draw {
class Pixmap;
class SvgRenderer;
}

namespace kn::engraving {
class ImageStoreItem;

enum class ImageType : char {
    NONE, RASTER, SVG
};

//---------------------------------------------------------
//   @@ Image
//---------------------------------------------------------

class Image final : public BSymbol, public kfw::Contextable
{
    OBJECT_ALLOCATOR(engraving, Image)
    DECLARE_CLASSOF(ElementType::IMAGE)

    kfw::GlobalInject<kfw::draw::IImageProvider> imageProvider;

public:
    Image(EngravingItem* parent);
    Image(const Image&);
    ~Image();

    Image* clone() const override { return new Image(*this); }

    bool loadFromStore(const std::string& storePath);
    bool loadFromFile(kfw::io::path_t path);
    bool loadFromData(const std::string& suffix, const kfw::ByteArray&);

    void init();

    bool isImageFramed() const;
    double imageAspectRatio() const;
    void setSize(const SizeF& s) { m_size = s; }
    const SizeF& size() const { return m_size; }
    void updateImageHeight(const double& height);
    void updateImageWidth(const double& width);
    double imageHeight() const;
    double imageWidth() const;
    bool lockAspectRatio() const { return m_lockAspectRatio; }
    void setLockAspectRatio(bool v) { m_lockAspectRatio = v; }
    bool autoScale() const { return m_autoScale; }
    void setAutoScale(bool v) { m_autoScale = v; }
    ImageStoreItem* storeItem() const { return m_storeItem; }
    bool sizeIsSpatium() const { return m_sizeIsSpatium; }
    void setSizeIsSpatium(bool val) { m_sizeIsSpatium = val; }

    const std::string& storePath() const { return m_storePath; }
    void setStorePath(const std::string& p) { m_storePath = p; }

    PropertyValue getProperty(Pid) const override;
    bool setProperty(Pid propertyId, const PropertyValue&) override;
    PropertyValue propertyDefault(Pid id) const override;

    SizeF imageSize() const;

    void setImageType(ImageType);
    ImageType imageType() const { return m_imageType; }
    bool isValid() const { return m_rasterDoc || m_svgDoc; }

    kfw::draw::SvgRenderer* svgRenderer() const { return m_svgDoc; }
    const std::shared_ptr<kfw::draw::Pixmap>& rasterImage() const { return m_rasterDoc; }

    bool needStartEditingAfterSelecting() const override { return true; }
    int gripsCount() const override { return 2; }
    Grip initialEditModeGrip() const override { return Grip(1); }
    Grip defaultGrip() const override { return Grip(1); }
    std::vector<PointF> gripsPositions(const EditData&) const override;

    SizeF pixel2size(const SizeF& s) const;
    SizeF size2pixel(const SizeF& s) const;

    const kfw::draw::Pixmap& buffer() const { return m_buffer; }
    void setBuffer(const kfw::draw::Pixmap& p) const { m_buffer = p; }
    bool dirty() const { return m_dirty; }
    void setDirty(bool val) const { m_dirty = val; }

private:

    bool isEditable() const override { return true; }
    void startDragGrip(EditData&) override;
    void dragGrip(EditData& ed) override;
    std::vector<LineF> gripAnchorLines(Grip) const override { return std::vector<LineF>(); }

    ImageStoreItem* m_storeItem = nullptr;
    std::string m_storePath; // the path of the img in the ImageStore
    mutable kfw::draw::Pixmap m_buffer; // cached rendering
    SizeF m_size;                   // in mm or spatium units
    bool m_lockAspectRatio = false;
    bool m_autoScale = false;           // fill parent frame
    bool m_sizeIsSpatium = false;
    mutable bool m_dirty = false;

    std::shared_ptr<kfw::draw::Pixmap> m_rasterDoc;
    kfw::draw::SvgRenderer* m_svgDoc = nullptr;

    ImageType m_imageType = ImageType::NONE;
};
}
