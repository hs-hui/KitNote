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

#ifndef KN_ENGRAVING_PLAYBACKCONTEXT_H
#define KN_ENGRAVING_PLAYBACKCONTEXT_H

#include "mpe/mpetypes.h"
#include "mpe/events.h"

#include "../types/types.h"

namespace kn::engraving {
class EngravingItem;
class Segment;
class Dynamic;
class Hairpin;
class PlayTechAnnotation;
class SoundFlag;
class Score;
class MeasureRepeat;
class TextBase;
class ChordRest;
class RepeatSegment;

class PlaybackContext
{
public:
    kfw::mpe::dynamic_level_t appliableDynamicLevel(const track_idx_t trackIdx, const int nominalPositionTick) const;

    std::pair<kfw::mpe::timestamp_t, PlayingTechniqueType> playingTechnique(const Score* score, const int nominalPositionTick) const;
    kfw::mpe::timestamp_t findPlayingTechniqueTimestamp(const Score* score, PlayingTechniqueType type, const int startFromTick) const;

    std::map<kfw::mpe::timestamp_t, kfw::mpe::SoundPresetChangeEventList> soundPresets(const Score* score) const;
    kfw::mpe::SoundPresetChangeEventList soundPresets(const track_idx_t trackIdx, const int nominalPositionTick) const;

    std::map<kfw::mpe::timestamp_t, kfw::mpe::TextArticulationEventList> textArticulations(const Score* score) const;
    kfw::mpe::TextArticulationEvent textArticulation(const track_idx_t trackIdx, const int nominalPositionTick) const;

    std::map<kfw::mpe::timestamp_t, kfw::mpe::SyllableEventList> syllables(const Score* score) const;
    kfw::mpe::SyllableEvent syllable(const track_idx_t trackIdx, const int nominalPositionTick) const;

    kfw::mpe::DynamicLevelLayers dynamicLevelLayers(const Score* score) const;

    void update(const ID partId, const Score* score, bool expandRepeats = true);
    void clear();

    bool hasSoundFlags() const;

private:
    struct DynamicInfo {
        kfw::mpe::dynamic_level_t level = 0;
        int priority = -1;
    };

    using DynamicMap = std::map<int /*nominalPositionTick*/, DynamicInfo>;
    using DynamicsByTrack = std::map<track_idx_t, DynamicMap>;

    using SoundPresetsMap = std::map<int /*nominalPositionTick*/, kfw::mpe::SoundPresetChangeEventList>;
    using SoundPresetsByTrack = std::map<track_idx_t, SoundPresetsMap>;

    using TextArticulationMap = std::map<int /*nominalPositionTick*/, kfw::mpe::TextArticulationEvent>;
    using TextArticulationsByTrack = std::map<track_idx_t, TextArticulationMap>;

    using SyllableMap = std::map<int /*nominalPositionTick*/, kfw::mpe::SyllableEvent>;
    using SyllablesByTrack = std::map<track_idx_t, SyllableMap>;

    using PlayTechniquesMap = std::map<int /*nominalPositionTick*/, kn::engraving::PlayingTechniqueType>;

    using SoundFlagMap = std::unordered_map<staff_idx_t, const SoundFlag*>;

    kfw::mpe::dynamic_level_t nominalDynamicLevel(const track_idx_t trackIdx, const int positionTick) const;

    void updateDynamicMap(const Dynamic* dynamic, const Segment* segment, const int segmentPositionTick);
    void updatePlayTechMap(const PlayTechAnnotation* annotation, const int segmentPositionTick);
    void updateSoundPresetAndTextArticulationMap(const SoundFlagMap& flagsOnSegment, const int segmentPositionTick);
    void updateSyllableMap(const TextBase* text, const int segmentPositionTick);

    void handleSpanners(const ID partId, const Score* score, const int segmentStartTick, const int segmentEndTick,
                        const int tickPositionOffset);
    void handleHairpin(const Hairpin* hairpin, const int tickPositionOffset);
    void handleSegmentAnnotations(const ID partId, const Segment* segment, const int segmentPositionTick);
    void handleSegmentElements(const RepeatSegment* repeat, const Segment* segment, const int segmentPositionTick,
                               std::vector<const MeasureRepeat*>& foundMeasureRepeats);
    void handleMeasureRepeats(const std::vector<const MeasureRepeat*>& measureRepeats, const int tickPositionOffset);

    void applyDynamic(const EngravingItem* dynamicItem, kfw::mpe::dynamic_level_t dynamicLevel, const int positionTick);

    bool shouldSkipTrack(const track_idx_t trackIdx) const;
    bool hasOnlyOneLyricsVerse(const RepeatSegment* repeat, const track_idx_t track) const;

    track_idx_t m_partStartTrack = 0;
    track_idx_t m_partEndTrack = 0;

    std::set<voice_idx_t> m_usedVoices;
    DynamicsByTrack m_dynamicsByTrack;
    SoundPresetsByTrack m_soundPresetsByTrack;
    TextArticulationsByTrack m_textArticulationsByTrack;
    SyllablesByTrack m_syllablesByTrack;
    PlayTechniquesMap m_playTechniquesMap;

    std::unordered_map<const ChordRest*, int> m_currentVerseNumByChordRest;
    std::unordered_map<track_idx_t, std::set<int /*tick*/> > m_multiVerseLyricsPositionMap;
};

using PlaybackContextPtr = std::shared_ptr<PlaybackContext>;
}

#endif // KN_ENGRAVING_PLAYBACKCONTEXT_H
