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
#include "kitnoterecording.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "audio/common/audiofilereader.h"

#include "engraving/dom/measure.h"
#include "engraving/dom/repeatlist.h"
#include "engraving/dom/score.h"
#include "engraving/dom/tempo.h"

#include "log.h"

using namespace kn::engraving;

namespace kn::playback {
const InstrumentTrackId& recordingTrackId()
{
    // (part 999 is the metronome's too: no part of a score has that id with this instrument)
    static const InstrumentTrackId id { 999, u"kitnote-recording" };
    return id;
}

ScoreRecording scoreRecording(const Score* score)
{
    ScoreRecording recording;
    if (!score) {
        return recording;
    }
    recording.path = score->metaTag(RECORDING_TAG).toQString().trimmed();
    recording.offset = score->metaTag(RECORDING_OFFSET_TAG).toQString().trimmed().toDouble();
    return recording;
}

std::map<kfw::String, kfw::String> metaTagsWithRecording(const Score* score, const ScoreRecording& recording)
{
    std::map<kfw::String, kfw::String> tags = score ? score->metaTags() : std::map<kfw::String, kfw::String>();
    if (recording.isSet()) {
        tags[RECORDING_TAG] = kfw::String::fromQString(QDir::fromNativeSeparators(recording.path));
        tags[RECORDING_OFFSET_TAG] = kfw::String::fromQString(QString::number(recording.offset, 'f', 3));
    } else {
        tags.erase(RECORDING_TAG);
        tags.erase(RECORDING_OFFSET_TAG);
    }
    return tags;
}

QString recordingPathToStore(const QString& absolutePath, const QString& scoreFilePath)
{
    const QString file = QDir::fromNativeSeparators(QFileInfo(absolutePath).absoluteFilePath());
    if (scoreFilePath.isEmpty()) {
        return file;
    }
    const QString relative = QDir(QFileInfo(scoreFilePath).absolutePath()).relativeFilePath(file);
    if (relative.startsWith(QLatin1String("..")) || QFileInfo(relative).isAbsolute()) {
        return file;
    }
    return relative;
}

QString resolveRecordingPath(const QString& storedPath, const QString& scoreFilePath)
{
    if (storedPath.isEmpty()) {
        return QString();
    }
    const QString scoreFolder = scoreFilePath.isEmpty() ? QString() : QFileInfo(scoreFilePath).absolutePath();
    const QFileInfo stored(storedPath);
    if (stored.isAbsolute()) {
        if (stored.isFile()) {
            return stored.absoluteFilePath();
        }
    } else if (!scoreFolder.isEmpty() && QFileInfo(scoreFolder + QLatin1Char('/') + storedPath).isFile()) {
        return QFileInfo(scoreFolder + QLatin1Char('/') + storedPath).absoluteFilePath();
    }
    if (!scoreFolder.isEmpty() && QFileInfo(scoreFolder + QLatin1Char('/') + stored.fileName()).isFile()) {
        return QFileInfo(scoreFolder + QLatin1Char('/') + stored.fileName()).absoluteFilePath();
    }
    return QString();
}

double measureOneSecs(const Score* score)
{
    if (!score || !score->firstMeasure()) {
        return 0.0;
    }
    // a drum key and a pickup share the number 1 with the measure after them: the last of them
    const Measure* first = score->firstMeasure();
    for (const Measure* measure = score->firstMeasure(); measure && measure->no() == 0; measure = measure->nextMeasure()) {
        first = measure;
    }
    const int utick = score->repeatList().tick2utick(first->tick().ticks());
    return score->utick2utime(utick) * score->tempomap()->tempoMultiplier().val;
}


//! General MIDI drums: the bass drums low; hi-hats, cymbals, ride and the like high; the rest (snares, toms,
//! hand percussion) in the middle
static int drumBand(int midiNote)
{
    switch (midiNote) {
    case 35: case 36:
        return 0;
    case 42: case 44: case 46: case 49: case 51: case 52: case 53: case 55: case 57: case 59:
        return 2;
    default:
        return 1;
    }
}

RecordingNotes scoreNotes(const notation::INotationPlaybackPtr& playback, double tempoMultiplier)
{
    RecordingNotes notes;
    if (!playback) {
        return notes;
    }
    for (const InstrumentTrackId& trackId : playback->existingTrackIdSet()) {
        if (trackId == playback->metronomeTrackId() || playback->isChordSymbolsTrack(trackId)) {
            continue;
        }
        const kfw::mpe::PlaybackEventsMap& events = playback->trackPlaybackData(trackId).originEvents;
        for (auto it = events.cbegin(); it != events.cend(); ++it) {
            for (const kfw::mpe::PlaybackEvent& event : it->second) {
                if (!std::holds_alternative<kfw::mpe::NoteEvent>(event)) {
                    continue;
                }
                const kfw::mpe::NoteEvent& note = std::get<kfw::mpe::NoteEvent>(event);
                const int midiNote = kfw::mpe::ZERO_PITCH_LEVEL_MIDI_EQUIVALENT
                                     + note.pitchCtx().nominalPitchLevel / kfw::mpe::PITCH_LEVEL_STEP;
                notes.push_back({ it->first * 1e-6 * tempoMultiplier, drumBand(midiNote) });
            }
        }
    }
    std::sort(notes.begin(), notes.end(), [](const RecordingNote& a, const RecordingNote& b) { return a.time < b.time; });
    return notes;
}

// --- finding the offset -------------------------------------------------------------------------------

namespace {
constexpr float PI = 3.14159265358979f;
constexpr int BANDS = 3;
constexpr double HOP = 0.005;           // seconds between frames
constexpr double TOLERANCE = 0.03;      // a stroke this near a note is the note's

void fft(std::vector<std::complex<float> >& a)
{
    const size_t n = a.size();
    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            std::swap(a[i], a[j]);
        }
    }
    for (size_t len = 2; len <= n; len <<= 1) {
        const float angle = -2.f * PI / static_cast<float>(len);
        const std::complex<float> step(std::cos(angle), std::sin(angle));
        for (size_t i = 0; i < n; i += len) {
            std::complex<float> w(1.f, 0.f);
            for (size_t k = 0; k < len / 2; ++k) {
                const std::complex<float> u = a[i + k];
                const std::complex<float> v = a[i + k + len / 2] * w;
                a[i + k] = u + v;
                a[i + k + len / 2] = u - v;
                w *= step;
            }
        }
    }
}

//! what a recording's strokes look like, band by band: how far each frame's rise of the spectrum stands out
//! (z, spread over the tolerance), and the strokes themselves (peaks, in seconds)
struct Strokes {
    std::array<std::vector<float>, BANDS> z;
    std::array<std::vector<double>, BANDS> times;
    double delay = 0.0;     // from a frame's start to the stroke it shows
    double frameSecs = HOP; // from a frame to the next (a whole number of samples)
};

Strokes findStrokes(const kfw::audio::AudioFileData& audio)
{
    Strokes result;
    const double rate = audio.sampleRate;
    const size_t hop = std::max<size_t>(1, static_cast<size_t>(std::lround(rate * HOP)));
    const size_t window = rate > 64000 ? 2048 : 1024;
    const int64_t frames = static_cast<int64_t>(audio.frames());
    if (frames < static_cast<int64_t>(window * 4)) {
        return result;
    }

    std::vector<float> mono(frames);
    for (int64_t f = 0; f < frames; ++f) {
        float sum = 0.f;
        for (unsigned int c = 0; c < audio.channels; ++c) {
            sum += audio.samples[f * audio.channels + c];
        }
        mono[f] = sum / (32768.f * audio.channels);
    }

    std::vector<float> hann(window);
    for (size_t i = 0; i < window; ++i) {
        hann[i] = 0.5f - 0.5f * std::cos(2.f * PI * i / window);
    }

    // the bands: bass drum, snare and toms, cymbals
    const size_t bins = window / 2 + 1;
    std::vector<int> bandOfBin(bins, -1);
    for (size_t b = 0; b < bins; ++b) {
        const double frequency = b * rate / window;
        bandOfBin[b] = frequency >= 30 && frequency < 150 ? 0 : frequency >= 150 && frequency < 4000 ? 1 : frequency >= 6000 ? 2 : -1;
    }

    const size_t count = (frames - window) / hop;
    std::array<std::vector<float>, BANDS> flux;
    for (auto& band : flux) {
        band.assign(count, 0.f);
    }
    std::vector<float> previous(bins, 0.f);
    std::vector<std::complex<float> > buffer(window);
    for (size_t k = 0; k < count; ++k) {
        const float* x = mono.data() + k * hop;
        for (size_t i = 0; i < window; ++i) {
            buffer[i] = std::complex<float>(x[i] * hann[i], 0.f);
        }
        fft(buffer);
        for (size_t b = 0; b < bins; ++b) {
            const float level = std::log1p(100.f * std::abs(buffer[b]));
            if (k > 0 && level > previous[b] && bandOfBin[b] >= 0) {
                flux[bandOfBin[b]][k] += level - previous[b];
            }
            previous[b] = level;
        }
    }
    mono.clear();

    const size_t around = static_cast<size_t>(0.5 * rate / hop);       // a second of music, for the mean and spread
    const size_t near = static_cast<size_t>(TOLERANCE * rate / hop);   // a stroke is the largest this near
    const size_t spread = static_cast<size_t>(0.015 * rate / hop);
    // The rise is greatest when the stroke is three quarters into the window (where Hann climbs fastest),
    // less the few milliseconds a drum's attack takes to build: 6 ms put recordings made of KitNote's own
    // drums within 2 ms of where they belong (tools/kitnote/smoke_test.py checks it).
    result.delay = 0.75 * window / rate - 0.006;
    result.frameSecs = hop / rate;

    for (int band = 0; band < BANDS; ++band) {
        const std::vector<float>& f = flux[band];
        std::vector<double> sum(count + 1, 0.0);
        std::vector<double> squares(count + 1, 0.0);
        for (size_t k = 0; k < count; ++k) {
            sum[k + 1] = sum[k] + f[k];
            squares[k + 1] = squares[k] + double(f[k]) * f[k];
        }
        const double overall = sum[count] / count;

        std::vector<float> z(count, -1.f);
        for (size_t k = 0; k < count; ++k) {
            const size_t a = k > around ? k - around : 0;
            const size_t b = std::min(count, k + around + 1);
            const double mean = (sum[b] - sum[a]) / (b - a);
            const double deviation = std::sqrt(std::max(0.0, (squares[b] - squares[a]) / (b - a) - mean * mean));
            z[k] = static_cast<float>(std::clamp((f[k] - mean) / (deviation + 1e-3 * overall + 1e-9), -1.0, 3.0));

            // a stroke: the largest within the tolerance, above the music around by its spread
            if (k == 0 || k + 1 >= count || f[k] <= mean + deviation) {
                continue;
            }
            bool largest = true;
            for (size_t j = (k > near ? k - near : 0); j <= std::min(count - 1, k + near) && largest; ++j) {
                largest = f[j] < f[k] || (f[j] == f[k] && j >= k);
            }
            if (largest) {
                const double l = f[k - 1];
                const double c = f[k];
                const double r = f[k + 1];
                const double curve = l - 2 * c + r;
                const double shift = curve < 0 ? std::clamp(0.5 * (l - r) / curve, -0.5, 0.5) : 0.0;
                result.times[band].push_back((k + shift) * hop / rate + result.delay);
            }
        }

        // a note a little off its stroke still meets it
        std::vector<float>& spreadZ = result.z[band];
        spreadZ.assign(count, -1.f);
        for (size_t k = 0; k < count; ++k) {
            float best = -1.f;
            for (size_t j = (k > spread ? k - spread : 0); j <= std::min(count - 1, k + spread); ++j) {
                best = std::max(best, z[j]);
            }
            spreadZ[k] = best;
        }
    }
    return result;
}

bool readFile(const QString& path, kfw::audio::AudioFileData& audio)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    const QByteArray bytes = file.readAll();
    return kfw::audio::readAudioFile(reinterpret_cast<const uint8_t*>(bytes.constData()), static_cast<size_t>(bytes.size()), audio);
}
}

std::optional<double> findRecordingOffset(const QString& audioPath, const RecordingNotes& notes, double measureOneSecs)
{
    if (notes.empty()) {
        return std::nullopt;
    }

    // the drums alone, when the transcription kept them, align more surely than the whole song
    const QFileInfo info(audioPath);
    const QString stem = info.absolutePath() + QLatin1Char('/') + info.completeBaseName() + QLatin1String(" stems/drums.wav");
    kfw::audio::AudioFileData audio;
    if (!(QFileInfo(stem).isFile() && readFile(stem, audio)) && !readFile(audioPath, audio)) {
        LOGE() << "KitNote: the recording could not be read: " << audioPath;
        return std::nullopt;
    }

    const Strokes strokes = findStrokes(audio);
    audio = kfw::audio::AudioFileData();
    const int64_t frames = static_cast<int64_t>(strokes.z[0].size());
    if (frames == 0) {
        return std::nullopt;
    }

    // a note's frame: the one whose window shows its stroke
    std::vector<int64_t> noteFrame;
    noteFrame.reserve(notes.size());
    for (const RecordingNote& note : notes) {
        noteFrame.push_back(std::llround((note.time - strokes.delay) / strokes.frameSecs));
    }
    const int64_t firstNote = noteFrame.front();
    const int64_t lastNote = noteFrame.back();

    // every offset that keeps half of the notes in the recording
    double bestScore = -2.0;
    int64_t bestLag = 0;
    for (int64_t lag = -lastNote; lag <= frames - firstNote; ++lag) {
        double sum = 0.0;
        size_t inside = 0;
        for (size_t i = 0; i < notes.size(); ++i) {
            const int64_t frame = noteFrame[i] + lag;
            if (frame >= 0 && frame < frames) {
                sum += strokes.z[notes[i].band][frame];
                ++inside;
            }
        }
        if (inside * 2 < notes.size()) {
            continue;
        }
        const double score = sum / notes.size();
        if (score > bestScore) {
            bestScore = score;
            bestLag = lag;
        }
    }

    // refined: the middle of how far each note (of these) is from the stroke of its band nearest to it
    auto refine = [&](double lag, const std::vector<size_t>& which, size_t& met) {
        for (int pass = 0; pass < 2; ++pass) {
            std::vector<double> differences;
            for (size_t i : which) {
                const std::vector<double>& times = strokes.times[notes[i].band];
                const double expected = notes[i].time + lag;
                auto it = std::lower_bound(times.begin(), times.end(), expected - TOLERANCE);
                double nearest = TOLERANCE;
                for (; it != times.end() && *it <= expected + TOLERANCE; ++it) {
                    if (std::abs(*it - expected) < std::abs(nearest)) {
                        nearest = *it - expected;
                    }
                }
                if (std::abs(nearest) < TOLERANCE) {
                    differences.push_back(nearest);
                }
            }
            met = differences.size();
            if (differences.empty()) {
                break;
            }
            std::nth_element(differences.begin(), differences.begin() + differences.size() / 2, differences.end());
            lag += differences[differences.size() / 2];
        }
        return lag;
    };

    std::vector<size_t> all(notes.size());
    for (size_t i = 0; i < notes.size(); ++i) {
        all[i] = i;
    }
    size_t met = 0;
    double lag = refine(bestLag * strokes.frameSecs, all, met);

    // a score of another song: hardly a note meets a stroke
    if (bestScore < 0.3 || met * 4 < notes.size()) {
        LOGI() << "KitNote: the score does not fit the recording (" << bestScore << ", " << met << " of " << notes.size() << " notes)";
        return std::nullopt;
    }

    // Measure 1 is where the score and the song are to meet. When the score's tempo is a little off the
    // song's, the offset that fits the whole song is the one of its middle: so it is looked for again, near
    // that one, with the notes of the first 20 seconds alone, and refined on them.
    std::vector<size_t> early;
    for (size_t i = 0; i < notes.size() && notes[i].time < notes.front().time + 20.0; ++i) {
        early.push_back(i);
    }
    if (early.size() >= 16) {
        const int64_t reach = std::llround(0.3 / strokes.frameSecs);
        double bestEarly = -2.0;
        int64_t earlyLag = bestLag;
        for (int64_t candidate = bestLag - reach; candidate <= bestLag + reach; ++candidate) {
            double sum = 0.0;
            for (size_t i : early) {
                const int64_t frame = noteFrame[i] + candidate;
                if (frame >= 0 && frame < frames) {
                    sum += strokes.z[notes[i].band][frame];
                }
            }
            if (sum > bestEarly) {
                bestEarly = sum;
                earlyLag = candidate;
            }
        }
        size_t earlyMet = 0;
        const double earlyOffset = refine(earlyLag * strokes.frameSecs, early, earlyMet);
        if (earlyMet * 2 >= early.size()) {
            lag = earlyOffset;
        }
    }

    LOGI() << "KitNote: recording aligned (" << bestScore << ", " << met << " of " << notes.size() << " notes): measure 1 at "
           << lag + measureOneSecs << " s";
    return lag + measureOneSecs;
}
}
