/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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
#include "recordingtempo.h"

#include <algorithm>
#include <cmath>
#include <optional>

#include "editscoreproperties.h"

#include "../dom/factory.h"
#include "../dom/masterscore.h"
#include "../dom/measure.h"
#include "../dom/segment.h"
#include "../dom/tempotext.h"
#include "../types/constants.h"

#include "log.h"

namespace kn::engraving {
namespace {
//! a measure of a drum key (see NotationInteraction::insertDrumKey): not counted, with the key's hidden mark
bool isDrumKeyMeasure(const Measure* measure)
{
    if (!measure->irregular()) {
        return false;
    }
    for (const Segment* segment = measure->first(SegmentType::ChordRest); segment; segment = segment->next(SegmentType::ChordRest)) {
        for (const EngravingItem* annotation : segment->annotations()) {
            if (annotation->isTempoText() && toTempoText(annotation)->isTempoPrimo() && !annotation->visible()) {
                return true;
            }
        }
    }
    return false;
}

//! the measures of the music, past a drum key
std::vector<Measure*> musicMeasures(const Score* score)
{
    std::vector<Measure*> measures;
    Measure* measure = score->firstMeasure();
    while (measure && isDrumKeyMeasure(measure)) {
        measure = measure->nextMeasure();
    }
    for (; measure; measure = measure->nextMeasure()) {
        measures.push_back(measure);
    }
    return measures;
}

TempoText* visibleTempoMark(const Measure* measure)
{
    const Segment* segment = measure->first(SegmentType::ChordRest);
    if (!segment) {
        return nullptr;
    }
    for (EngravingItem* annotation : segment->annotations()) {
        if (annotation->isTempoText() && annotation->visible()) {
            return toTempoText(annotation);
        }
    }
    return nullptr;
}

//! in 6/8, 9/8 and 12/8 the beat - and the tempo mark - is a dotted quarter
bool isCompound(const Measure* measure)
{
    const Fraction signature = measure->timesig();
    return signature.denominator() == 8 && signature.numerator() % 3 == 0 && signature.numerator() > 3;
}

//! the tempo as a mark writes it: quarters a minute, or dotted quarters in compound time, to so many decimals
double writtenTempo(double quarters, bool compound, int decimals)
{
    const double scale = std::pow(10.0, decimals);
    const double beat = compound ? quarters / 1.5 : quarters;
    return std::round(beat * scale) / scale;
}

//! "♩ = 94" (as NotationInteraction::setTempoAtMeasure writes it), or "♩. = 62" in compound time
String tempoMarkText(double quarters, bool compound, int decimals)
{
    const String number = String::number(writtenTempo(quarters, compound, decimals), decimals);
    return compound ? String(u"<sym>metNoteQuarterUp</sym><sym>space</sym><sym>metAugmentationDot</sym> = %1").arg(number)
           : String(u"<sym>metNoteQuarterUp</sym> = %1").arg(number);
}

//! a tempo mark at the beginning of the measure, at the tempo written (quarters a minute)
TempoText* addTempoMark(Score* score, Measure* measure, double quarters, bool visible)
{
    Segment* segment = measure->first(SegmentType::ChordRest);
    if (!segment) {
        return nullptr;
    }
    const bool compound = isCompound(measure);
    const int decimals = visible ? 0 : 2;
    TempoText* tempo = Factory::createTempoText(segment);
    tempo->setParent(segment);
    tempo->setTrack(0);
    tempo->setXmlText(tempoMarkText(quarters, compound, decimals));
    tempo->setFollowText(true);
    tempo->setTempo(BeatsPerSecond(writtenTempo(quarters, compound, decimals) * (compound ? 1.5 : 1.0) / 60.0));
    tempo->setVisible(visible);
    score->undoAddElement(tempo);
    return tempo;
}

void setRecordingTempoTag(Score* score, std::optional<double> quarters)
{
    MasterScore* master = score->masterScore();
    std::map<String, String> tags = master->metaTags();
    if (quarters) {
        const String value = String::number(*quarters, 3);
        const auto it = tags.find(RECORDING_TEMPO_TAG);
        if (it != tags.end() && it->second == value) {
            return;
        }
        tags[RECORDING_TEMPO_TAG] = value;
    } else if (!tags.erase(RECORDING_TEMPO_TAG)) {
        return;
    }
    master->undo(new ChangeMetaTags(master, tags));
}

int removeHiddenMarks(Score* score, const std::vector<Measure*>& measures)
{
    int removed = 0;
    for (Measure* measure : measures) {
        for (Segment* segment = measure->first(SegmentType::ChordRest); segment; segment = segment->next(SegmentType::ChordRest)) {
            const std::vector<EngravingItem*> annotations = segment->annotations();     // a copy: removing changes the list
            for (EngravingItem* annotation : annotations) {
                if (isRecordingTempoMark(annotation)) {
                    score->undoRemoveElement(annotation);
                    ++removed;
                }
            }
        }
    }
    return removed;
}
}

bool isRecordingTempoMark(const EngravingItem* item)
{
    if (!item || !item->isTempoText() || item->visible()) {
        return false;
    }
    const TempoText* tempo = toTempoText(item);
    return tempo->isNormal() && tempo->segment() && tempo->segment()->rtick().isZero();
}

bool tempoFollowsRecording(const Score* score)
{
    return score && score->masterScore()->metaTags().count(RECORDING_TEMPO_TAG) > 0;
}

int recordingTempoMarkCount(const Score* score)
{
    if (!tempoFollowsRecording(score)) {
        return 0;
    }
    int count = 0;
    for (const Measure* measure : musicMeasures(score->masterScore())) {
        if (const Segment* segment = measure->first(SegmentType::ChordRest)) {
            count += static_cast<int>(std::count_if(segment->annotations().begin(), segment->annotations().end(), isRecordingTempoMark));
        }
    }
    return count;
}

// The measures are followed one after another, the time each begins in the playback (t) set against the time
// it begins in the recording (T): the tempo in force goes on while it keeps the measures within the
// tolerance; where it no longer does, a hidden mark gives the tempo that keeps the most measures after it
// within it (the line through them that fits best). A measure with a visible mark plays at its tempo (the
// first, and those of the passages at another tempo): the measure after it corrects what that tempo,
// rounded, is off.
RecordingTempo followRecordingTimes(Score* score, const std::vector<double>& times)
{
    RecordingTempo result;
    score = score->masterScore();
    const std::vector<Measure*> measures = musicMeasures(score);
    if (measures.empty() || times.size() < 2) {
        return result;
    }
    const size_t n = std::min(measures.size(), times.size() - 1);
    for (size_t i = 0; i < n; ++i) {
        if (!(times[i + 1] > times[i])) {
            LOGE() << "KitNote: the times of the measures do not follow one another";
            return result;
        }
    }

    // the length of each measure in quarters, from the start of the first (X), and its tempo in the recording
    std::vector<double> X(n + 1, 0.0);
    std::vector<double> measured(n);       // quarters a minute
    for (size_t i = 0; i < n; ++i) {
        const double quarters = measures[i]->ticks().ticks() / static_cast<double>(Constants::DIVISION);
        X[i + 1] = X[i] + quarters;
        measured[i] = quarters * 60.0 / (times[i + 1] - times[i]);
    }

    removeHiddenMarks(score, measures);

    // the first measure: its tempo mark, the tempo usual in the recording when it has none
    TempoText* first = visibleTempoMark(measures[0]);
    if (!first) {
        std::vector<double> sorted = measured;
        std::nth_element(sorted.begin(), sorted.begin() + sorted.size() / 2, sorted.end());
        first = addTempoMark(score, measures[0], sorted[sorted.size() / 2], true);
        if (!first) {
            return result;
        }
    }

    // the measures with a visible mark (the user's own included): they play at its tempo (quarters a second)
    std::vector<std::optional<double> > forced(n);
    forced[0] = first->tempo().val;
    for (size_t i = 1; i < n; ++i) {
        if (const TempoText* mark = visibleTempoMark(measures[i])) {
            forced[i] = mark->tempo().val;
        }
    }

    // A passage of some measures clearly at another tempo (a slower bridge, a ritardando at the end) gets a
    // mark of its own, at the tempo of its first measure, rounded; one measure off (a fill rushed, a beat
    // the beat model missed) does not.
    double marked = *forced[0] * 60.0;
    for (size_t i = 1; i + RECORDING_SECTION_MEASURES <= n;) {
        if (forced[i]) {
            marked = *forced[i] * 60.0;
            ++i;
            continue;
        }
        bool slower = true;
        bool faster = true;
        for (size_t j = i; j < i + RECORDING_SECTION_MEASURES; ++j) {
            slower &= measured[j] <= marked * (1.0 - RECORDING_SECTION_CHANGE) && (j == i || !forced[j]);
            faster &= measured[j] >= marked * (1.0 + RECORDING_SECTION_CHANGE) && (j == i || !forced[j]);
        }
        if (!slower && !faster) {
            ++i;
            continue;
        }
        const bool compound = isCompound(measures[i]);
        const double quarters = writtenTempo(measured[i], compound, 0) * (compound ? 1.5 : 1.0);
        if (TempoText* mark = addTempoMark(score, measures[i], quarters, true)) {
            forced[i] = mark->tempo().val;
            marked = quarters;
            result.marks.push_back({ measures[i]->no() + 1, static_cast<int>(writtenTempo(quarters, compound, 0)) });
        }
        i += RECORDING_SECTION_MEASURES;
    }

    std::vector<double> played(n, 0.0);    // the tempo each measure plays at, quarters a second
    const double tolerance = RECORDING_TEMPO_TOLERANCE;

    // the last measure, up to "limit", that "tempo" from measure "from" (beginning at "start") keeps in time
    auto keepsTime = [&](size_t from, double start, double tempo, size_t limit) {
        size_t to = from;
        while (to < limit && std::abs(start + (X[to + 1] - X[from]) / tempo - times[to + 1]) <= tolerance) {
            ++to;
        }
        return to;
    };
    auto play = [&](size_t from, size_t to, double tempo) {
        for (size_t i = from; i < to; ++i) {
            played[i] = tempo;
        }
    };

    double t = times[0];
    double tempo = *forced[0];
    size_t i = 0;
    while (i < n) {
        size_t limit = i + 1;      // the next measure with a visible mark, or the end
        while (limit < n && !forced[limit]) {
            ++limit;
        }

        if (forced[i]) {
            tempo = *forced[i];
            const size_t to = std::max(keepsTime(i, t, tempo, limit), i + 1);
            play(i, to, tempo);
            t += (X[to] - X[i]) / tempo;
            i = to;
            continue;
        }

        const size_t kept = keepsTime(i, t, tempo, limit);
        if (kept > i) {
            play(i, kept, tempo);
            t += (X[kept] - X[i]) / tempo;
            i = kept;
            continue;
        }

        // the line from here through the measures after that fits them best (least squares), as far as it
        // keeps them all within the tolerance
        size_t fitTo = i + 1;
        double secondsPerQuarter = (times[i + 1] - t) / (X[i + 1] - X[i]);
        double sxx = 0.0;
        double sxy = 0.0;
        for (size_t j = i + 1; j <= limit; ++j) {
            const double dx = X[j] - X[i];
            sxx += dx * dx;
            sxy += dx * (times[j] - t);
            const double slope = sxy / sxx;
            bool fits = slope > 0.0;
            for (size_t m = i + 1; m <= j && fits; ++m) {
                fits = std::abs(t + (X[m] - X[i]) * slope - times[m]) <= tolerance;
            }
            if (fits) {
                fitTo = j;
                secondsPerQuarter = slope;
            } else if (j > fitTo + RECORDING_SECTION_MEASURES) {
                break;      // (it does not fit again further on)
            }
        }
        if (secondsPerQuarter <= 0.0) {
            play(i, i + 1, tempo);
            t += (X[i + 1] - X[i]) / tempo;
            ++i;
            continue;
        }

        // the tempo as the mark writes it (two decimals): the measures are timed with that
        const bool compound = isCompound(measures[i]);
        const double quarters = writtenTempo(60.0 / secondsPerQuarter, compound, 2) * (compound ? 1.5 : 1.0);
        if (std::abs(quarters / 60.0 - tempo) * 60.0 >= 0.005 && addTempoMark(score, measures[i], quarters, false)) {
            tempo = quarters / 60.0;
            ++result.hiddenMarks;
        }
        play(i, fitTo, tempo);
        t += (X[fitTo] - X[i]) / tempo;
        i = fitTo;
    }

    t = times[0];
    for (size_t m = 0; m < n; ++m) {
        t += (X[m + 1] - X[m]) / played[m];
        result.largestDrift = std::max(result.largestDrift, std::abs(t - times[m + 1]));
    }

    result.measures = static_cast<int>(n);
    result.tempo = *forced[0] * 60.0;
    setRecordingTempoTag(score, result.tempo);
    return result;
}

int removeRecordingTempo(Score* score)
{
    score = score->masterScore();
    const int removed = removeHiddenMarks(score, musicMeasures(score));
    setRecordingTempoTag(score, std::nullopt);
    return removed;
}

void keepRecordingTempoScaled(Score* score)
{
    if (!score) {
        return;
    }
    MasterScore* master = score->masterScore();
    const auto tag = master->metaTags().find(RECORDING_TEMPO_TAG);
    if (tag == master->metaTags().end()) {
        return;
    }
    bool ok = false;
    const double base = tag->second.toDouble(&ok);
    const std::vector<Measure*> measures = musicMeasures(master);
    const TempoText* first = measures.empty() ? nullptr : visibleTempoMark(measures.front());
    if (!ok || base <= 0.0 || !first) {
        return;     // (no first mark: nothing to scale by)
    }
    const double now = first->tempo().val * 60.0;
    if (std::abs(now - base) < 0.0005) {
        return;
    }

    const double factor = now / base;
    for (size_t i = 0; i < measures.size(); ++i) {
        const bool compound = isCompound(measures[i]);
        for (Segment* segment = measures[i]->first(SegmentType::ChordRest); segment;
             segment = segment->next(SegmentType::ChordRest)) {
            for (EngravingItem* annotation : segment->annotations()) {
                if (!annotation->isTempoText() || annotation == first) {
                    continue;
                }
                TempoText* mark = toTempoText(annotation);
                if (!mark->isNormal() || !mark->playTempoText()) {
                    continue;       // ("a tempo" and "tempo primo" go back to a tempo that is scaled already)
                }
                const double old = mark->tempo().val * 60.0;
                const int decimals = mark->visible() ? 0 : 2;
                // (through the base class: TempoText keeps its own version of this to itself)
                EngravingObject* object = mark;
                if (mark->xmlText() == tempoMarkText(old, compound, decimals)) {
                    const double quarters = writtenTempo(old * factor, compound, decimals) * (compound ? 1.5 : 1.0);
                    object->undoChangeProperty(Pid::TEXT, tempoMarkText(quarters, compound, decimals));
                    object->undoChangeProperty(Pid::TEMPO, BeatsPerSecond(quarters / 60.0));
                } else if (!mark->followText()) {
                    object->undoChangeProperty(Pid::TEMPO, BeatsPerSecond(old * factor / 60.0));
                }
                // (a mark in words of its own, its tempo from them, stays as it is)
            }
        }
    }
    setRecordingTempoTag(master, now);
}
}
