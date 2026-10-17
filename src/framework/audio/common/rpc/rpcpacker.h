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

#include <cassert>

#include "global/serialization/msgpack_forward.h"
#include "audio/common/audiotypes.h"

#include "log.h"

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioEngineConfig& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioEngineConfig& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::OutputSpec& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::OutputSpec& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::PlaybackStatus& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::PlaybackStatus& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioResourceType& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioResourceType& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioFxCategory& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioFxCategory& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioResourceMeta& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioResourceMeta& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioFxParams& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioFxParams& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AuxSendParams& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AuxSendParams& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioSourceParams& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioSourceParams& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioOutputParams& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioOutputParams& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioParams& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioParams& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::SoundPreset& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::SoundPreset& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::SoundTrackType& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::SoundTrackType& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioSampleFormat& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioSampleFormat& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::SoundTrackFormat& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::SoundTrackFormat& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::SaveSoundTrackStage& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::SaveSoundTrackStage& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioSignalVal& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioSignalVal& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::InputProcessingProgress::ChunkInfo& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::InputProcessingProgress::ChunkInfo& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::InputProcessingProgress::ProgressInfo& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::InputProcessingProgress::ProgressInfo& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::InputProcessingProgress::StatusInfo& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::InputProcessingProgress::StatusInfo& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::TransportEvent& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::TransportEvent& value);

// MPE
// PlaybackEvent
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArrangementContext& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArrangementContext& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::PitchContext& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::PitchContext& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArrangementPattern& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArrangementPattern& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::PitchPattern& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::PitchPattern& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ExpressionPattern& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ExpressionPattern& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArticulationPatternSegment& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArticulationPatternSegment& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArticulationType& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArticulationType& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArticulationMeta& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArticulationMeta& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArticulationAppliedData& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArticulationAppliedData& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ExpressionContext& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ExpressionContext& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::NoteEvent& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::NoteEvent& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::TextArticulationEvent& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::TextArticulationEvent& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::SoundPresetChangeEvent& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::SoundPresetChangeEvent& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::SyllableEvent& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::SyllableEvent& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ControllerChangeEvent& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ControllerChangeEvent& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::PlaybackEvent& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::PlaybackEvent& value);

void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::SoundCategory& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::SoundCategory& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::PlaybackSetupData& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::PlaybackSetupData& value);
void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::PlaybackData& value);
void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::PlaybackData& value);

#include "global/serialization/msgpack.h"

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioEngineConfig& value)
{
    p.process(value.autoProcessOnlineSoundsInBackground, value.isLazyProcessingOfOnlineSoundsEnabled,
              value.useSoundFontLowPassFilter);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioEngineConfig& value)
{
    p.process(value.autoProcessOnlineSoundsInBackground, value.isLazyProcessingOfOnlineSoundsEnabled,
              value.useSoundFontLowPassFilter);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::OutputSpec& value)
{
    p.process(value.sampleRate, value.samplesPerChannel, value.audioChannelCount);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::OutputSpec& value)
{
    p.process(value.sampleRate, value.samplesPerChannel, value.audioChannelCount);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::PlaybackStatus& value)
{
    p.process(static_cast<int8_t>(value));
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::PlaybackStatus& value)
{
    int8_t val = 0;
    p.process(val);
    value = static_cast<kfw::audio::PlaybackStatus>(val);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioResourceType& value)
{
    p.process(static_cast<int8_t>(value));
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioResourceType& value)
{
    int type = 0;
    p.process(type);
    value = static_cast<kfw::audio::AudioResourceType>(type);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioFxCategory& value)
{
    p.process(static_cast<int>(value));
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioFxCategory& value)
{
    int cat = 0;
    p.process(cat);
    value = static_cast<kfw::audio::AudioFxCategory>(cat);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioResourceMeta& value)
{
    p.process(value.id, value.type, value.vendor, value.attributes, value.hasNativeEditorSupport);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioResourceMeta& value)
{
    p.process(value.id, value.type, value.vendor, value.attributes, value.hasNativeEditorSupport);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioFxParams& value)
{
    p.process(value.categories, value.chainOrder, value.resourceMeta, value.configuration, value.active);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioFxParams& value)
{
    p.process(value.categories, value.chainOrder, value.resourceMeta, value.configuration, value.active);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AuxSendParams& value)
{
    p.process(value.signalAmount, value.active);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AuxSendParams& value)
{
    p.process(value.signalAmount, value.active);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioOutputParams& value)
{
    p.process(value.fxChain, value.volume, value.balance,  value.auxSends, value.solo, value.muted, value.forceMute);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioOutputParams& value)
{
    p.process(value.fxChain, value.volume, value.balance, value.auxSends, value.solo, value.muted, value.forceMute);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioParams& value)
{
    p.process(value.in, value.out);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioParams& value)
{
    p.process(value.in, value.out);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::SoundPreset& value)
{
    p.process(value.code, value.name, value.isDefault, value.attributes);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::SoundPreset& value)
{
    p.process(value.code, value.name, value.isDefault, value.attributes);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioSourceParams& value)
{
    p.process(value.resourceMeta, value.configuration);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioSourceParams& value)
{
    p.process(value.resourceMeta, value.configuration);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::SoundTrackType& value)
{
    p.process(static_cast<int>(value));
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::SoundTrackType& value)
{
    int type = 0;
    p.process(type);
    value = static_cast<kfw::audio::SoundTrackType>(type);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioSampleFormat& value)
{
    p.process(static_cast<int>(value));
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioSampleFormat& value)
{
    int format = 0;
    p.process(format);
    value = static_cast<kfw::audio::AudioSampleFormat>(format);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::SoundTrackFormat& value)
{
    p.process(value.type, value.outputSpec, value.sampleFormat, value.bitRate,
              value.leadingSilenceDuration, value.trailingSilenceDuration);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::SoundTrackFormat& value)
{
    p.process(value.type, value.outputSpec, value.sampleFormat, value.bitRate,
              value.leadingSilenceDuration, value.trailingSilenceDuration);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::SaveSoundTrackStage& value)
{
    p.process(static_cast<int>(value));
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::SaveSoundTrackStage& value)
{
    int stage = 0;
    p.process(stage);
    value = static_cast<kfw::audio::SaveSoundTrackStage>(stage);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::AudioSignalVal& value)
{
    p.process(value.pressure);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::AudioSignalVal& value)
{
    p.process(value.pressure);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::InputProcessingProgress::ChunkInfo& value)
{
    p.process(value.start, value.end);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::InputProcessingProgress::ChunkInfo& value)
{
    p.process(value.start, value.end);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::InputProcessingProgress::ProgressInfo& value)
{
    p.process(value.current, value.total);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::InputProcessingProgress::ProgressInfo& value)
{
    p.process(value.current, value.total);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::InputProcessingProgress::StatusInfo& value)
{
    p.process(static_cast<uint8_t>(value.status), value.errorCode, value.errorText, value.data);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::InputProcessingProgress::StatusInfo& value)
{
    uint8_t status = 0;
    p.process(status, value.errorCode, value.errorText, value.data);
    value.status = static_cast<kfw::audio::InputProcessingProgress::Status>(status);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::audio::TransportEvent& value)
{
    p.process(static_cast<uint8_t>(value.type));

    if (value.type == kfw::audio::TransportEvent::Type::Seek) {
        const auto& seek = std::get<kfw::audio::TransportEvent::SeekData>(value.data);
        p.process(seek.position);
    }
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::audio::TransportEvent& value)
{
    uint8_t type = 0;
    p.process(type);
    value.type = static_cast<kfw::audio::TransportEvent::Type>(type);

    if (value.type == kfw::audio::TransportEvent::Type::Seek) {
        kfw::audio::TransportEvent::SeekData seek;
        p.process(seek.position);
        value.data = seek;
    }
}

// MPE
inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArrangementContext& value)
{
    p.process(value.nominalTimestamp, value.actualTimestamp, value.nominalDuration, value.actualDuration,
              value.voiceLayerIndex, value.staffLayerIndex, value.bps);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArrangementContext& value)
{
    p.process(value.nominalTimestamp, value.actualTimestamp, value.nominalDuration, value.actualDuration,
              value.voiceLayerIndex, value.staffLayerIndex, value.bps);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::PitchContext& value)
{
    p.process(value.nominalPitchLevel, value.pitchCurve);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::PitchContext& value)
{
    p.process(value.nominalPitchLevel, value.pitchCurve);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArrangementPattern& value)
{
    p.process(value.durationFactor, value.timestampOffset);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArrangementPattern& value)
{
    p.process(value.durationFactor, value.timestampOffset);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::PitchPattern& value)
{
    p.process(value.pitchOffsetMap);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::PitchPattern& value)
{
    p.process(value.pitchOffsetMap);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ExpressionPattern& value)
{
    p.process(value.dynamicOffsetMap);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ExpressionPattern& value)
{
    p.process(value.dynamicOffsetMap);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArticulationPatternSegment& value)
{
    p.process(value.arrangementPattern, value.pitchPattern, value.expressionPattern);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArticulationPatternSegment& value)
{
    p.process(value.arrangementPattern, value.pitchPattern, value.expressionPattern);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArticulationType& value)
{
    p.process(static_cast<int8_t>(value));
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArticulationType& value)
{
    int8_t type = 0;
    p.process(type);
    value = static_cast<kfw::mpe::ArticulationType>(type);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArticulationMeta& value)
{
    p.process(value.type, value.pattern, value.timestamp, value.overallDuration,
              value.overallPitchChangesRange, value.overallDynamicChangesRange);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArticulationMeta& value)
{
    p.process(value.type, value.pattern, value.timestamp, value.overallDuration,
              value.overallPitchChangesRange, value.overallDynamicChangesRange);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ArticulationAppliedData& value)
{
    p.process(value.meta, value.appliedPatternSegment, value.occupiedFrom, value.occupiedTo,
              value.occupiedPitchChangesRange, value.occupiedDynamicChangesRange);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ArticulationAppliedData& value)
{
    p.process(value.meta, value.appliedPatternSegment, value.occupiedFrom, value.occupiedTo,
              value.occupiedPitchChangesRange, value.occupiedDynamicChangesRange);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ExpressionContext& value)
{
    p.process(value.articulations, value.nominalDynamicLevel, value.expressionCurve, value.velocityOverride);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ExpressionContext& value)
{
    p.process(value.articulations, value.nominalDynamicLevel, value.expressionCurve, value.velocityOverride);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::NoteEvent& value)
{
    p.process(value.arrangementCtx(), value.pitchCtx(), value.expressionCtx());
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::NoteEvent& value)
{
    kfw::mpe::ArrangementContext arrCtx;
    kfw::mpe::PitchContext pitchCtx;
    kfw::mpe::ExpressionContext exprCtx;
    p.process(arrCtx, pitchCtx, exprCtx);
    value = kfw::mpe::NoteEvent(std::move(arrCtx), std::move(pitchCtx), std::move(exprCtx));
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::TextArticulationEvent& value)
{
    p.process(value.text, value.layerIdx, value.flags);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::TextArticulationEvent& value)
{
    p.process(value.text, value.layerIdx, value.flags);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::SoundPresetChangeEvent& value)
{
    p.process(value.code, value.layerIdx);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::SoundPresetChangeEvent& value)
{
    p.process(value.code, value.layerIdx);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::SyllableEvent& value)
{
    p.process(value.text, value.layerIdx, value.flags);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::SyllableEvent& value)
{
    p.process(value.text, value.layerIdx, value.flags);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::ControllerChangeEvent& value)
{
    p.process(static_cast<int8_t>(value.type), value.val, value.layerIdx);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::ControllerChangeEvent& value)
{
    int8_t type = 0;
    p.process(type, value.val, value.layerIdx);
    value.type = static_cast<kfw::mpe::ControllerChangeEvent::Type>(type);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::PlaybackEvent& value)
{
    uint8_t idx = static_cast<uint8_t>(value.index());
    p.process(idx);

    switch (idx) {
    case 0:
        // no data
        break;
    case 1: {
        const kfw::mpe::NoteEvent& event = std::get<kfw::mpe::NoteEvent>(value);
        p.process(event);
    } break;
    case 2: {
        const kfw::mpe::TextArticulationEvent& event = std::get<kfw::mpe::TextArticulationEvent>(value);
        p.process(event);
    } break;
    case 3: {
        const kfw::mpe::SoundPresetChangeEvent& event = std::get<kfw::mpe::SoundPresetChangeEvent>(value);
        p.process(event);
    } break;
    case 4: {
        const kfw::mpe::SyllableEvent& event = std::get<kfw::mpe::SyllableEvent>(value);
        p.process(event);
    } break;
    case 5: {
        const kfw::mpe::ControllerChangeEvent& event = std::get<kfw::mpe::ControllerChangeEvent>(value);
        p.process(event);
    } break;
    default: {
        assert(false && "unknown PlaybackEvent variant index");
    }
    }
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::PlaybackEvent& value)
{
    uint8_t idx = 0;
    p.process(idx);

    switch (idx) {
    case 0:
        // no data
        break;
    case 1: {
        kfw::mpe::NoteEvent event;
        p.process(event);
        value = event;
    } break;
    case 2: {
        kfw::mpe::TextArticulationEvent event;
        p.process(event);
        value = event;
    } break;
    case 3: {
        kfw::mpe::SoundPresetChangeEvent event;
        p.process(event);
        value = event;
    } break;
    case 4: {
        kfw::mpe::SyllableEvent event;
        p.process(event);
        value = event;
    } break;
    case 5: {
        kfw::mpe::ControllerChangeEvent event;
        p.process(event);
        value = event;
    } break;
    default: {
        assert(false && "unknown PlaybackEvent variant index");
    }
    }
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::SoundCategory& value)
{
    p.process(static_cast<int8_t>(value));
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::SoundCategory& value)
{
    int8_t type = 0;
    p.process(type);
    value = static_cast<kfw::mpe::SoundCategory>(type);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::PlaybackSetupData& value)
{
    p.process(value.id, value.category, value.subCategories,
              value.supportsSingleNoteDynamics, value.musicXmlSoundId, value.scoreId);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::PlaybackSetupData& value)
{
    p.process(value.id, value.category, value.subCategories,
              value.supportsSingleNoteDynamics, value.musicXmlSoundId, value.scoreId);
}

inline void pack_custom(kfw::msgpack::Packer& p, const kfw::mpe::PlaybackData& value)
{
    p.process(value.originEvents, value.setupData, value.dynamics);
}

inline void unpack_custom(kfw::msgpack::UnPacker& p, kfw::mpe::PlaybackData& value)
{
    p.process(value.originEvents, value.setupData, value.dynamics);
}

namespace kfw::audio::rpc {
using Options = msgpack::Options;
class RpcPacker
{
public:
    RpcPacker() = default;

    template<class ... Types>
    static ByteArray pack(const Options& opt, const Types&... args)
    {
        // clear but keep capacity
        buffer.clear();
        // makes sense if the reserve is greater than the current capacity
        buffer.reserve(std::max(opt.rezerveSize, DEFAULT_CAPACITY));
        msgpack::pack(buffer, args ...);

        IF_ASSERT_FAILED(buffer.size() > 0) {
            return ByteArray();
        }

        return ByteArray(&buffer[0], buffer.size());
    }

    template<class ... Types>
    static ByteArray pack(const Types&... args)
    {
        return RpcPacker::pack(Options {}, args ...);
    }

    template<class ... Types>
    static bool unpack(const ByteArray& data, Types&... args)
    {
        return msgpack::unpack(data, args ...);
    }

    static constexpr size_t DEFAULT_CAPACITY = 1024 * 200;
    static inline thread_local std::vector<uint8_t> buffer = {};
};
}
