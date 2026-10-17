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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include "mpe/events.h"

#include "../dom/articulation.h"
#include "../dom/chord.h"
#include "../dom/note.h"
#include "../dom/pitchspelling.h"
#include "../dom/sig.h"
#include "../dom/staff.h"

#include "utils/arrangementutils.h"
#include "utils/pitchutils.h"
#include "playbackcontext.h"

namespace kn::engraving {
struct RenderingContext {
    RenderingContext() = default;

    kfw::mpe::timestamp_t nominalTimestamp = 0;
    kfw::mpe::duration_t nominalDuration = 0;
    kfw::mpe::dynamic_level_t nominalDynamicLevel = 0;
    int nominalPositionStartTick = 0;
    int nominalPositionEndTick = 0;
    int nominalDurationTicks = 0;
    int positionTickOffset = 0;

    BeatsPerSecond beatsPerSecond = 0;
    TimeSigFrac timeSignatureFraction;

    kfw::mpe::ArticulationMap commonArticulations;

    const Score* score = nullptr;
    const kfw::mpe::ArticulationsProfilePtr profile;
    const PlaybackContextPtr playbackCtx;

    bool isValid() const
    {
        return score
               && profile
               && playbackCtx
               && beatsPerSecond > 0
               && nominalDuration > 0
               && nominalDurationTicks > 0;
    }
};

inline RenderingContext buildRenderingCtx(const Chord* chord, const int tickPositionOffset,
                                          const kfw::mpe::ArticulationsProfilePtr profile, const PlaybackContextPtr playbackCtx,
                                          const kfw::mpe::ArticulationMap& articulations = {})
{
    int chordPosTick = chord->tick().ticks();
    int chordDurationTicks = chord->actualTicks().ticks();
    int chordPosTickWithOffset = chordPosTick + tickPositionOffset;

    const Score* score = chord->score();

    auto chordTnD = timestampAndDurationFromStartAndDurationTicks(score, chordPosTick, chordDurationTicks, tickPositionOffset);

    BeatsPerSecond bps = score->tempomap()->multipliedTempo(chordPosTick);
    TimeSigFrac timeSignatureFraction = score->sigmap()->timesig(chordPosTick).timesig();

    RenderingContext ctx{ chordTnD.timestamp,
                          chordTnD.duration,
                          playbackCtx->appliableDynamicLevel(chord->track(), chordPosTickWithOffset),
                          chordPosTick,
                          chordPosTick + chordDurationTicks,
                          chordDurationTicks,
                          tickPositionOffset,
                          bps,
                          timeSignatureFraction,
                          articulations,
                          score,
                          profile,
                          playbackCtx };

    return ctx;
}

// KitNote: on a drum staff, the open sign ("o") over a closed or pedal hi-hat plays the open hi-hat,
// and the closed sign ("+") over an open hi-hat plays the closed one. Returns the General MIDI pitch
// to play instead of the note's own, or -1.
inline int hiHatPitchFromSign(const Note* note)
{
    static constexpr int CLOSED_HI_HAT = 42;
    static constexpr int PEDAL_HI_HAT = 44;
    static constexpr int OPEN_HI_HAT = 46;

    const int pitch = note->pitch();
    if (pitch != CLOSED_HI_HAT && pitch != PEDAL_HI_HAT && pitch != OPEN_HI_HAT) {
        return -1;
    }

    const Chord* chord = note->chord();
    const Staff* staff = note->staff();
    if (!chord || !staff || !staff->isDrumStaff(chord->tick())) {
        return -1;
    }

    for (const Articulation* articulation : chord->articulations()) {
        if (articulation->symId() == SymId::brassMuteOpen && pitch != OPEN_HI_HAT) {
            return OPEN_HI_HAT;
        }
        if (articulation->symId() == SymId::brassMuteClosed && pitch == OPEN_HI_HAT) {
            return CLOSED_HI_HAT;
        }
    }

    return -1;
}

struct NominalNoteCtx {
    voice_idx_t voiceIdx = 0;
    staff_idx_t staffIdx = 0;
    kfw::mpe::timestamp_t timestamp = 0;
    kfw::mpe::duration_t duration = 0;
    BeatsPerSecond tempo = 0;
    kfw::mpe::dynamic_level_t dynamicLevel = 0;
    float userVelocityFraction = 0.f;

    kfw::mpe::pitch_level_t pitchLevel = 0;

    RenderingContext chordCtx;
    kfw::mpe::ArticulationMap articulations;

    explicit NominalNoteCtx(const Note* note, const RenderingContext& ctx)
        : voiceIdx(note->voice()),
        staffIdx(note->staffIdx()),
        timestamp(ctx.nominalTimestamp),
        duration(ctx.nominalDuration),
        tempo(ctx.beatsPerSecond),
        dynamicLevel(ctx.nominalDynamicLevel),
        userVelocityFraction(note->userVelocityFraction()),
        pitchLevel(notePitchLevel(note->playingTpc(),
                                  note->playingOctave(),
                                  note->playingTuning())),
        chordCtx(ctx),
        articulations(ctx.commonArticulations)
    {
        // KitNote: the grace note of a flam, drag or ruff is a light tap, four dynamics under the main note.
        const Chord* chord = note->chord();
        if (chord && chord->isGrace() && note->staff() && note->staff()->isDrumStaff(chord->tick())) {
            const kfw::mpe::dynamic_level_t lowered = std::max(kfw::mpe::dynamicLevelFromType(kfw::mpe::DynamicType::ppp),
                                                                static_cast<kfw::mpe::dynamic_level_t>(
                                                                    dynamicLevel - 4 * kfw::mpe::DYNAMIC_LEVEL_STEP));
            dynamicLevel = std::min(dynamicLevel, lowered); // never louder than it was
        }

        const int hiHatPitch = hiHatPitchFromSign(note);
        if (hiHatPitch >= 0) {
            const int tpc = pitch2tpc(hiHatPitch, Key::C, Prefer::NEAREST);
            pitchLevel = notePitchLevel(tpc, playingOctave(hiHatPitch, tpc));
        }
    }
};

inline kfw::mpe::NoteEvent buildNoteEvent(const NominalNoteCtx& ctx, const kfw::mpe::PitchCurve& pitchCurve = {})
{
    return kfw::mpe::NoteEvent(ctx.timestamp,
                                ctx.duration,
                                static_cast<kfw::mpe::voice_layer_idx_t>(ctx.voiceIdx),
                                static_cast<kfw::mpe::staff_layer_idx_t>(ctx.staffIdx),
                                ctx.pitchLevel,
                                ctx.dynamicLevel,
                                ctx.articulations,
                                ctx.tempo.val,
                                ctx.userVelocityFraction,
                                pitchCurve);
}
}
