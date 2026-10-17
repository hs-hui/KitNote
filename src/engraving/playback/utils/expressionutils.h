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

#ifndef KN_ENGRAVING_DYNAMICUTILS_H
#define KN_ENGRAVING_DYNAMICUTILS_H

#include "mpe/mpetypes.h"

#include "../types/types.h"

namespace kn::engraving {
constexpr kfw::mpe::dynamic_level_t NATURAL_DYNAMIC_LEVEL = kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::Natural);

inline const std::unordered_map<DynamicType, kfw::mpe::dynamic_level_t> ORDINARY_DYNAMIC_LEVELS {
    { DynamicType::N, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::ppppppppp) },
    { DynamicType::PPPPPP, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::pppppp) },
    { DynamicType::PPPPP, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::ppppp) },
    { DynamicType::PPPP, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::pppp) },
    { DynamicType::PPP, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::ppp) },
    { DynamicType::PP, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::pp) },
    { DynamicType::P, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::p) },
    { DynamicType::MP, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::mp) },
    { DynamicType::MF, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::mf) },
    { DynamicType::F, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::f) },
    { DynamicType::FF, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::ff) },
    { DynamicType::FFF, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::fff) },
    { DynamicType::FFFF, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::ffff) },
    { DynamicType::FFFFF, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::fffff) },
    { DynamicType::FFFFFF, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::ffffff) }
};

inline bool isOrdinaryDynamicType(const DynamicType type)
{
    return ORDINARY_DYNAMIC_LEVELS.find(type) != ORDINARY_DYNAMIC_LEVELS.cend();
}

inline kfw::mpe::dynamic_level_t dynamicLevelFromOrdinaryType(const DynamicType type,
                                                               const kfw::mpe::dynamic_level_t defLevel = NATURAL_DYNAMIC_LEVEL)
{
    auto search = ORDINARY_DYNAMIC_LEVELS.find(type);
    if (search != ORDINARY_DYNAMIC_LEVELS.cend()) {
        return search->second;
    }

    return defLevel;
}

inline const std::unordered_map<DynamicType, kfw::mpe::dynamic_level_t> SINGLE_NOTE_DYNAMIC_LEVELS {
    { DynamicType::SF, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::f) },
    { DynamicType::SFZ, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::f) },
    { DynamicType::SFF, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::ff) },
    { DynamicType::SFFZ, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::ff) },
    { DynamicType::SFFF, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::fff) },
    { DynamicType::SFFFZ, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::fff) },
    { DynamicType::RFZ, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::f) },
    { DynamicType::RF, kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::f) }
};

inline bool isSingleNoteDynamicType(const DynamicType type)
{
    return SINGLE_NOTE_DYNAMIC_LEVELS.find(type) != SINGLE_NOTE_DYNAMIC_LEVELS.cend();
}

inline kfw::mpe::dynamic_level_t dynamicLevelFromSingleNoteType(const DynamicType type,
                                                                 const kfw::mpe::dynamic_level_t defLevel = NATURAL_DYNAMIC_LEVEL)
{
    auto search = SINGLE_NOTE_DYNAMIC_LEVELS.find(type);
    if (search != SINGLE_NOTE_DYNAMIC_LEVELS.cend()) {
        return search->second;
    }

    return defLevel;
}

struct CompoundDynamic {
    DynamicType from = DynamicType::OTHER;
    DynamicType to = DynamicType::OTHER;

    bool isValid() const
    {
        return from != DynamicType::OTHER && to != DynamicType::OTHER;
    }
};

inline const std::unordered_map<DynamicType, CompoundDynamic> COMPOUND_DYNAMICS {
    { DynamicType::FP, { DynamicType::F, DynamicType::P } },
    { DynamicType::PF, { DynamicType::P, DynamicType::F } },
    { DynamicType::SFP, { DynamicType::F, DynamicType::P } },
    { DynamicType::SFPP, { DynamicType::F, DynamicType::PP } }
};

inline bool isCompoundDynamicType(const DynamicType type)
{
    return COMPOUND_DYNAMICS.find(type) != COMPOUND_DYNAMICS.cend();
}

inline const CompoundDynamic& compoundDynamicFromType(const DynamicType type)
{
    auto search = COMPOUND_DYNAMICS.find(type);
    if (search != COMPOUND_DYNAMICS.cend()) {
        return search->second;
    }

    static CompoundDynamic empty;
    return empty;
}

inline kfw::mpe::ArticulationType articulationFromPlayTechType(const PlayingTechniqueType technique)
{
    switch (technique) {
    case PlayingTechniqueType::Natural: return kfw::mpe::ArticulationType::Standard;
    case PlayingTechniqueType::Pizzicato: return kfw::mpe::ArticulationType::Pizzicato;
    case PlayingTechniqueType::Open: return kfw::mpe::ArticulationType::Open;
    case PlayingTechniqueType::Mute: return kfw::mpe::ArticulationType::Mute;
    case PlayingTechniqueType::Tremolo: return kfw::mpe::ArticulationType::Tremolo64th;
    case PlayingTechniqueType::Detache: return kfw::mpe::ArticulationType::Detache;
    case PlayingTechniqueType::Martele: return kfw::mpe::ArticulationType::Martele;
    case PlayingTechniqueType::ColLegno: return kfw::mpe::ArticulationType::ColLegno;
    case PlayingTechniqueType::SulPonticello: return kfw::mpe::ArticulationType::SulPont;
    case PlayingTechniqueType::SulTasto: return kfw::mpe::ArticulationType::SulTasto;
    case PlayingTechniqueType::Distortion: return kfw::mpe::ArticulationType::Distortion;
    case PlayingTechniqueType::Overdrive: return kfw::mpe::ArticulationType::Overdrive;
    case PlayingTechniqueType::Harmonics: return kfw::mpe::ArticulationType::Harmonic;
    case PlayingTechniqueType::JazzTone: return kfw::mpe::ArticulationType::JazzTone;
    case PlayingTechniqueType::Vibrato: return kfw::mpe::ArticulationType::Vibrato;
    case PlayingTechniqueType::Legato: return kfw::mpe::ArticulationType::Legato;
    case PlayingTechniqueType::HandbellsSwing: return kfw::mpe::ArticulationType::Swing;
    case PlayingTechniqueType::HandbellsSwingUp: return kfw::mpe::ArticulationType::Swing;
    case PlayingTechniqueType::HandbellsSwingDown: return kfw::mpe::ArticulationType::Swing;
    case PlayingTechniqueType::HandbellsEcho1: return kfw::mpe::ArticulationType::Echo;
    case PlayingTechniqueType::HandbellsEcho2: return kfw::mpe::ArticulationType::Echo;
    case PlayingTechniqueType::HandbellsLV: return kfw::mpe::ArticulationType::Pedal;
    case PlayingTechniqueType::HandbellsDamp: return kfw::mpe::ArticulationType::Standard;
    case PlayingTechniqueType::HandbellsR: return kfw::mpe::ArticulationType::Ring;
    case PlayingTechniqueType::Undefined: return kfw::mpe::ArticulationType::Undefined;
    }

    return kfw::mpe::ArticulationType::Undefined;
}

static const kfw::mpe::ArticulationTypeSet GRACE_NOTE_ARTICULATION_TYPES {
    kfw::mpe::ArticulationType::Acciaccatura,
    kfw::mpe::ArticulationType::PostAppoggiatura,
    kfw::mpe::ArticulationType::PreAppoggiatura,
};

inline bool isGraceNotePlacedBeforePrincipalNote(const kfw::mpe::ArticulationType type)
{
    return type == kfw::mpe::ArticulationType::Acciaccatura || type == kfw::mpe::ArticulationType::PreAppoggiatura;
}

inline void updateArticulationBoundaries(const kfw::mpe::ArticulationType type, const kfw::mpe::timestamp_t nominalTimestamp,
                                         const kfw::mpe::duration_t nominalDuration,
                                         kfw::mpe::ArticulationMap& articulations)
{
    if (articulations.empty()) {
        return;
    }

    const kfw::mpe::ArticulationAppliedData& articulationData = articulations.at(type);

    kfw::mpe::timestamp_t articulationOccupiedFrom = nominalTimestamp - articulationData.meta.timestamp;
    kfw::mpe::timestamp_t articulationOccupiedTo = nominalTimestamp + nominalDuration - articulationData.meta.timestamp;

    articulations.updateOccupiedRange(type,
                                      kfw::mpe::occupiedPercentage(articulationOccupiedFrom,
                                                                    articulationData.meta.overallDuration),
                                      kfw::mpe::occupiedPercentage(articulationOccupiedTo,
                                                                    articulationData.meta.overallDuration));
}
}

#endif // KN_ENGRAVING_DYNAMICUTILS_H
