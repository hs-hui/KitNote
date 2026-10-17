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

#include "notation/inotation.h"

namespace kn::engraving {
class Score;
class MStyle;
}

namespace kn::converter {
class NotationMeta
{
public:
    static kfw::RetVal<std::string> metaJson(notation::INotationPtr notation);

    static QJsonArray tracksJsonArray(notation::INotationPtr notation);

private:
    static QString title(const kn::engraving::Score* score);
    static QString subtitle(const kn::engraving::Score* score);
    static QString composer(const kn::engraving::Score* score);
    static QString poet(const kn::engraving::Score* score);
    static QString timesig(const kn::engraving::Score* score);
    static std::pair<int, QString> tempo(const kn::engraving::Score* score);
    static QJsonArray partsJsonArray(const kn::engraving::Score* score);
    static QJsonObject pageFormatJson(const kn::engraving::MStyle& style);
    static QJsonObject typeDataJson(kn::engraving::Score* score);
};
}
