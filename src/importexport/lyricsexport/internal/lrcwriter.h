/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "project/inotationwriter.h"

#include "../ilyricsexportconfiguration.h"

namespace kn::engraving {
class Score;
}

namespace kn::iex::lrcexport {
class LRCWriter : public project::INotationWriter, public kfw::Contextable
{
public:
    kfw::GlobalInject<kn::iex::lrcexport::ILyricsExportConfiguration> configuration;

public:
    LRCWriter(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    // Interface implementation
    std::vector<UnitType> supportedUnitTypes() const override;
    bool supportsUnitType(UnitType) const override;
    kfw::Ret write(notation::INotationPtr, kfw::io::IODevice&, const Options&) override;
    void writeMetadata(kfw::io::IODevice* device, const engraving::Score* score) const;
    kfw::Ret writeList(const notation::INotationPtrList&, kfw::io::IODevice&, const Options&) override;

    bool writeScore(kn::engraving::Score* score, const kfw::io::path_t& path, bool enhancedLrc);

private:
    kfw::Ret doWrite(kn::engraving::Score* score, kfw::io::IODevice*, bool);

    // Core lyric functionality
    std::map<double, QString> collectLyrics(const kn::engraving::Score*);
    QString formatTimestamp(double ms) const;
    void findTrackAndLyricToExport(const engraving::Score* score, kn::engraving::track_idx_t& trackNumber, int& lyricNumber);
};
} // namespace kn::iex::lrcexport
