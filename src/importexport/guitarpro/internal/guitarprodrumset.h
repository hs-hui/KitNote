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

#ifndef KN_IMPORTEXPORT_GUITARPRODRUMSET_H
#define KN_IMPORTEXPORT_GUITARPRODRUMSET_H

#include <unordered_map>
#include <string_view>

namespace kn::engraving {
class Drumset;
class Instrument;
}

namespace kn::iex::guitarpro::drumset {
enum class DrumSetType : uint16_t {
    DRUMS = 0, AGOGO, HAND_CLAP, TAMBOURINE,
    COWBELL, VIBRASLAP, BONGOS, CONGAS,
    TIMBALE, CABASA, MARACAS, WHISTLE,
    GUIRO, CLAVES, WOODBLOCK, CUICA,
    TRIANGLE, SHAKER, JINGLE_BELL, BELL_TREE,
    CASTANETS, SURDO
};

struct GpDrumSet {
    std::string_view name;
    DrumSetType idx;
    uint8_t numLines;
};

static const std::unordered_map<std::string_view, GpDrumSet> PERC_STAFF_LINES_FROM_INSTRUMENT = {
    { "Agogo", { "Agogo", DrumSetType::AGOGO, 2 } },
    { "Hand Clap", { "Hand Clap", DrumSetType::HAND_CLAP, 1 } },
    { "Tambourine", { "Tambourine", DrumSetType::TAMBOURINE, 1 } },
    { "Cowbell", { "Cowbell", DrumSetType::COWBELL, 3 } },
    { "Vibraslap", { "Vibraslap", DrumSetType::VIBRASLAP, 1 } },
    { "Bongos", { "Bongos", DrumSetType::BONGOS, 2 } },
    { "Congas", { "Congas", DrumSetType::CONGAS, 2 } },
    { "Timbale", { "Timbale", DrumSetType::TIMBALE, 2 } },
    { "Cabasa", { "Cabasa", DrumSetType::CABASA, 1 } },
    { "Maracas", { "Maracas", DrumSetType::MARACAS, 2 } },
    { "Whistle", { "Whistle", DrumSetType::WHISTLE, 2 } },
    { "Guiro", { "Guiro", DrumSetType::GUIRO, 2 } },
    { "Claves", { "Claves", DrumSetType::CLAVES, 1 } },
    { "Woodblock", { "Woodblock", DrumSetType::WOODBLOCK, 2 } },
    { "Cuica", { "Cuica", DrumSetType::CUICA, 1 } },
    { "Triangle", { "Triangle", DrumSetType::TRIANGLE, 1 } },
    { "Shaker", { "Shaker", DrumSetType::SHAKER, 1 } },
    { "Jingle Bell", { "Jingle Bell", DrumSetType::JINGLE_BELL, 1 } },
    { "Bell Tree", { "Bell Tree", DrumSetType::BELL_TREE, 1 } },
    { "Castanets", { "Castanets", DrumSetType::CASTANETS, 1 } },
    { "Surdo",  { "Surdo", DrumSetType::SURDO, 1 } },
};

inline kn::engraving::Drumset* gpDrumset = nullptr;
inline kn::engraving::Drumset* gpAgogoSet = nullptr;
inline kn::engraving::Drumset* gpHandClapSet = nullptr;
inline kn::engraving::Drumset* gpTambourineSet = nullptr;
inline kn::engraving::Drumset* gpCowbellSet = nullptr;
inline kn::engraving::Drumset* gpVibraslapSet = nullptr;
inline kn::engraving::Drumset* gpBongosSet = nullptr;
inline kn::engraving::Drumset* gpCongasSet = nullptr;
inline kn::engraving::Drumset* gpTimbalesSet = nullptr;
inline kn::engraving::Drumset* gpCabasaSet = nullptr;
inline kn::engraving::Drumset* gpMaracasSet = nullptr;
inline kn::engraving::Drumset* gpWhistleSet = nullptr;
inline kn::engraving::Drumset* gpGuiroSet = nullptr;
inline kn::engraving::Drumset* gpClavesSet = nullptr;
inline kn::engraving::Drumset* gpWoodblockSet = nullptr;
inline kn::engraving::Drumset* gpCuicaSet = nullptr;
inline kn::engraving::Drumset* gpTriangleSet = nullptr;
inline kn::engraving::Drumset* gpShakerSet = nullptr;
inline kn::engraving::Drumset* gpJingleBellSet = nullptr;
inline kn::engraving::Drumset* gpBellTreeSet = nullptr;
inline kn::engraving::Drumset* gpCastanetsSet = nullptr;
inline kn::engraving::Drumset* gpSurdoSet = nullptr;

void initGuitarProDrumset();
void initGuitarProPercussionSet(const GpDrumSet& ds);
void setInstrumentDrumset(kn::engraving::Instrument* instrument, const GpDrumSet& ds);
} // namespace kn::iex::guitarpro::drumset
#endif
