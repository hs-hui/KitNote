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

#include "project/imscmetareader.h"

#include "io/ifilesystem.h"
#include "modularity/ioc.h"

namespace kfw {
class XmlStreamReader;
}

namespace kn::project {
class MscMetaReader : public IMscMetaReader
{
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;

public:
    kfw::RetVal<ProjectMeta> readMeta(const kfw::io::path_t& filePath) const override;

private:

    struct RawMeta {
        QString titleTag;
        QString titleAttribute;
        QString titleStyle;
        QString titleStyleHtml;

        QString subtitleAttribute;
        QString subtitleStyle;
        QString subtitleStyleHtml;

        QString composerAttribute;
        QString composerStyle;
        QString composerStyleHtml;

        QString lyricistAttribute;
        QString lyricistStyle;
        QString lyricistStyleHtml;

        QString copyright;
        QString translator;
        QString arranger;
        QString creationDate;

        QVariantMap additionalTags;

        size_t partsCount = 0;
    };

    kfw::Ret prepareReader(const kfw::io::path_t& filePath, kn::engraving::MscReader& reader) const;

    void doReadMeta(kfw::XmlStreamReader& xmlReader, ProjectMeta& meta) const;
    RawMeta doReadBox(kfw::XmlStreamReader& xmlReader) const;
    RawMeta doReadRawMeta(kfw::XmlStreamReader& xmlReader) const;
    QString formatFromXml(const std::string& xml) const;

    QString format(const std::string& str) const;
    QString simplified(const QString& str) const;
    QString simplified(const std::string& str) const;
    std::string cutXmlTags(const std::string& str) const;

    QString readText(kfw::XmlStreamReader& xmlReader) const;
    QString readMetaTagText(kfw::XmlStreamReader& xmlReader) const;
};
}
