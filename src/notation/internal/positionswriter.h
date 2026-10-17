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

#include "modularity/ioc.h"
#include "importexport/imagesexport/iimagesexportconfiguration.h"
#include "project/inotationwriter.h"

namespace kfw {
class XmlStreamWriter;
}

namespace kn::engraving {
class Score;
}

namespace kn::notation {
class PositionsWriter : public project::INotationWriter, public kfw::Contextable
{
    kfw::GlobalInject<iex::imagesexport::IImagesExportConfiguration> imagesExportConfiguration;

public:
    enum class ElementType {
        SEGMENT,
        MEASURE
    };

    explicit PositionsWriter(const kfw::modularity::ContextPtr& ctx);
    explicit PositionsWriter(ElementType elementType, const kfw::modularity::ContextPtr& ctx);

    std::vector<UnitType> supportedUnitTypes() const override;
    bool supportsUnitType(UnitType unitType) const override;

    kfw::Ret write(notation::INotationPtr notation, kfw::io::IODevice& device, const Options& options = Options()) override;
    kfw::Ret writeList(const INotationPtrList& notations, kfw::io::IODevice& device, const Options& options = Options()) override;

private:
    qreal pngDpiResolution() const;
    QHash<void*, int> elementIds(const kn::engraving::Score* score) const;

    void writeElementsPositions(kfw::XmlStreamWriter& writer, const kn::engraving::Score* score) const;
    void writeSegmentsPositions(kfw::XmlStreamWriter& writer, const kn::engraving::Score* score) const;
    void writeMeasuresPositions(kfw::XmlStreamWriter& writer, const kn::engraving::Score* score) const;

    void writeEventsPositions(kfw::XmlStreamWriter& writer, const kn::engraving::Score* score) const;

    ElementType m_elementType = ElementType::SEGMENT;
};
}
