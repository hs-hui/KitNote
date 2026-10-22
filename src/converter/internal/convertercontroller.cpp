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
#include "engraving/types/typesconv.h" // KitNote
#include "convertercontroller.h"

#include <algorithm> // KitNote
#include <cmath> // KitNote

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QFileInfo> // KitNote
#include <QRegularExpression> // KitNote

#include "defer.h"
#include "global/io/file.h"
#include "global/io/dir.h"

#include "engraving/dom/masterscore.h"
#include "engraving/dom/box.h"
#include "engraving/dom/factory.h"
#include "engraving/dom/measure.h"
#include "engraving/dom/text.h"
#include "engraving/dom/timesig.h"
#include "engraving/dom/tempo.h"
#include "engraving/dom/tempotext.h" // KitNote
#include "engraving/editing/recordingtempo.h" // KitNote
#include "engraving/dom/volta.h" // KitNote
#include "engraving/dom/sig.h"
#include "engraving/dom/segment.h"
#include "engraving/editing/undo.h" // KitNote
#include "engraving/editing/editscoreproperties.h" // KitNote
#include "playback/kitnoterecording.h" // KitNote
#include "engraving/infrastructure/mscio.h"

#include "convertercodes.h"
#include "compat/backendapi.h"
#include "compat/notationmeta.h"
#include "converterutils.h"

#include "log.h"

using namespace kn::converter;
using namespace kn::project;
using namespace kn::notation;
using namespace kfw;
using namespace kfw::io;

static const std::string PDF_SUFFIX = "pdf";
static const std::string PNG_SUFFIX = "png";
static const std::string SVG_SUFFIX = "svg";
static const std::string MP3_SUFFIX = "mp3";

//! KitNote: the measures numbered "first" to "last" (as the score numbers them, from 1), or null.
//! A drum key and a pickup share the number of the measure after them: the later one is taken.
static std::pair<kn::engraving::Measure*, kn::engraving::Measure*> kitNoteMeasures(kn::engraving::Score* score, int first, int last)
{
    kn::engraving::Measure* from = nullptr;
    kn::engraving::Measure* to = nullptr;
    for (kn::engraving::Measure* measure = score->firstMeasure(); measure; measure = measure->nextMeasure()) {
        const int number = measure->no() + 1;
        if (number == first && (!from || from->no() + 1 == first)) {
            from = measure;
        }
        if (number == last) {
            to = measure;
        }
        if (number > last) {
            break;
        }
    }
    if (from && !to) {
        to = score->lastMeasure();      // "to the end"
    }
    if (from && to && to->tick() < from->tick()) {
        to = from;
    }
    return { from, to };
}

//! KitNote: a text of the title frame - the title, the composer - is rewritten, or added.
static void setKitNoteFrameText(const INotationPtr& notation, kn::engraving::TextStyleType style, const QString& value)
{
    kn::engraving::Score* score = notation->elements()->msScore();
    kn::engraving::MeasureBase* first = score->first();
    if (!first || !first->isVBox()) {
        return;
    }

    notation->undoStack()->prepareChanges(kfw::TranslatableString("undoableAction", "Edit text"));
    bool found = false;
    for (kn::engraving::EngravingItem* item : first->el()) {
        if (item->isText() && kn::engraving::toText(item)->textStyleType() == style) {
            kn::engraving::EngravingObject* object = item;
            object->undoChangeProperty(kn::engraving::Pid::TEXT, kfw::String::fromQString(value.toHtmlEscaped()));
            found = true;
        }
    }
    if (!found) {
        kn::engraving::Text* text = kn::engraving::Factory::createText(first, style);
        text->setParent(first);
        text->setXmlText(kfw::String::fromQString(value.toHtmlEscaped()));
        score->undoAddElement(text);
    }
    if (style == kn::engraving::TextStyleType::TITLE) {
        score->setMetaTag(u"workTitle", kfw::String::fromQString(value));
    } else if (style == kn::engraving::TextStyleType::COMPOSER) {
        score->setMetaTag(u"composer", kfw::String::fromQString(value));
    }
    notation->undoStack()->commitChanges();
}

//! KitNote: what an AI tool needs to know about a score, as JSON: the title, the tempo, the time
//! signature, the number of measures, the sections and the drum tab.
static QByteArray kitNoteScoreSummary(const INotationPtr& notation)
{
    kn::engraving::Score* score = notation->elements()->msScore();
    QJsonObject summary;

    QString title;
    if (score->first() && score->first()->isVBox()) {
        for (kn::engraving::EngravingItem* item : score->first()->el()) {
            if (item->isText() && kn::engraving::toText(item)->textStyleType() == kn::engraving::TextStyleType::TITLE) {
                title = kn::engraving::toText(item)->plainText().toQString();
            }
        }
    }
    summary["title"] = title;
    QString composer;
    if (score->first() && score->first()->isVBox()) {
        for (kn::engraving::EngravingItem* item : score->first()->el()) {
            if (item->isText() && kn::engraving::toText(item)->textStyleType() == kn::engraving::TextStyleType::COMPOSER) {
                composer = kn::engraving::toText(item)->plainText().toQString();
            }
        }
    }
    summary["composer"] = composer;
    const QString swing = notation->style()->styleValue(kn::engraving::Sid::swingUnit).value<kfw::String>().toQString();
    summary["swing"] = swing == "eighth" ? "8" : swing == "16th" ? "16" : "off";

    int measures = 0;
    for (kn::engraving::Measure* measure = score->firstMeasure(); measure; measure = measure->nextMeasure()) {
        measures = std::max(measures, measure->no() + 1);
    }
    summary["measures"] = measures;

    // the measures at the end with nothing in them (no note, no repeat sign): a score longer than its music
    int emptyAtEnd = 0;
    for (kn::engraving::Measure* measure = score->lastMeasure(); measure; measure = measure->prevMeasure()) {
        bool empty = true;
        for (kn::engraving::Segment* segment = measure->first(kn::engraving::SegmentType::ChordRest); segment && empty;
             segment = segment->next(kn::engraving::SegmentType::ChordRest)) {
            for (kn::engraving::EngravingItem* item : segment->elist()) {
                if (item && item->isChord()) {
                    empty = false;
                    break;
                }
            }
        }
        for (kn::engraving::staff_idx_t staff = 0; staff < score->nstaves() && empty; ++staff) {
            empty = !measure->isMeasureRepeatGroup(staff);
        }
        if (!empty) {
            break;
        }
        ++emptyAtEnd;
    }
    summary["empty_measures_at_end"] = emptyAtEnd;

    if (kn::engraving::Measure* last = score->lastMeasure()) {
        // the tempo and the time signature of the music, past a drum key
        const kn::engraving::Fraction tick = last->tick();
        const auto sig = score->sigmap()->timesig(tick).nominal();     // 4/4, not 1/1
        summary["time_signature"] = QStringLiteral("%1/%2").arg(sig.numerator()).arg(sig.denominator());
        // at measure 1 of the music (a drum key before it has a tempo of its own)
        const kn::engraving::Measure* first = kitNoteMeasures(score, 1, 1).first;
        const double quarters = score->tempomap()->tempo(first ? first->tick().ticks() : 0).val * 60.0;
        summary["tempo_bpm"] = qRound(quarters);
        // in 6/8, 9/8 and 12/8 the beat - and the tempo mark - is a dotted quarter
        if (sig.denominator() == 8 && sig.numerator() % 3 == 0 && sig.numerator() > 3) {
            summary["tempo_dotted_quarter"] = qRound(quarters / 1.5);
        }
    }

    // tempo changes partway through (the marks of the music, a drum key's hidden one left out), and the
    // repeat barlines - said only when there are any
    QJsonArray tempoMarks;
    QJsonArray repeats;
    int repeatStart = 0;        // the measure of the last start repeat, until its end repeat
    for (kn::engraving::Measure* measure = score->firstMeasure(); measure; measure = measure->nextMeasure()) {
        const int number = measure->no() + 1;
        for (kn::engraving::Segment* segment = measure->first(kn::engraving::SegmentType::ChordRest); segment;
             segment = segment->next(kn::engraving::SegmentType::ChordRest)) {
            for (kn::engraving::EngravingItem* annotation : segment->annotations()) {
                if (!annotation->isTempoText() || !annotation->visible()) {
                    continue;
                }
                const double quarters = kn::engraving::toTempoText(annotation)->tempo().val * 60.0;
                QJsonObject mark;
                mark["measure"] = number;
                mark["bpm"] = qRound(quarters);
                const kn::engraving::Fraction sig = measure->timesig();
                if (sig.denominator() == 8 && sig.numerator() % 3 == 0 && sig.numerator() > 3) {
                    mark["dotted_quarter"] = qRound(quarters / 1.5);
                }
                tempoMarks.append(mark);
            }
        }
        if (measure->repeatStart()) {
            repeatStart = number;
        }
        if (measure->repeatEnd()) {
            // (without a start repeat the music goes back to the beginning)
            QJsonObject repeat;
            repeat["first_measure"] = repeatStart > 0 ? repeatStart : 1;
            repeat["last_measure"] = number;
            repeat["play_count"] = std::max(2, measure->repeatCount());
            repeats.append(repeat);
            repeatStart = 0;
        }
    }
    if (repeatStart > 0) {
        QJsonObject repeat;
        repeat["first_measure"] = repeatStart;      // a start repeat with no end repeat after it
        repeats.append(repeat);
    }
    if (tempoMarks.size() > 1) {
        summary["tempo_marks"] = tempoMarks;
    }
    if (!repeats.isEmpty()) {
        summary["repeats"] = repeats;
    }

    // the endings (1st, 2nd ...) over measures
    QJsonArray voltas;
    for (const auto& entry : score->spanner()) {
        if (!entry.second || !entry.second->isVolta()) {
            continue;
        }
        const kn::engraving::Volta* volta = kn::engraving::toVolta(entry.second);
        const kn::engraving::Measure* start = score->tick2measure(volta->tick());
        const kn::engraving::Measure* end = score->tick2measure(volta->tick2() - kn::engraving::Fraction::eps());
        QJsonArray endings;
        for (int ending : volta->endings()) {
            endings.append(ending);
        }
        QJsonObject item;
        item["first_measure"] = start ? start->no() + 1 : 0;
        item["last_measure"] = end ? end->no() + 1 : 0;
        item["endings"] = endings;
        voltas.append(item);
    }
    if (!voltas.isEmpty()) {
        summary["voltas"] = voltas;
    }

    // the measure-repeat signs (%, and the signs of two and four measures): the drum tab shows what they
    // stand for, this says where they are
    QJsonArray repeatSigns;
    for (kn::engraving::Measure* measure = score->firstMeasure(); measure; measure = measure->nextMeasure()) {
        for (kn::engraving::staff_idx_t staff = 0; staff < score->nstaves(); ++staff) {
            if (measure->isMeasureRepeatGroup(staff) && measure->firstOfMeasureRepeatGroup(staff) == measure) {
                QJsonObject sign;
                sign["first_measure"] = measure->no() + 1;
                sign["measure_count"] = measure->measureRepeatNumMeasures(staff);
                repeatSigns.append(sign);
                break;
            }
        }
    }
    if (!repeatSigns.isEmpty()) {
        summary["repeat_signs"] = repeatSigns;
    }

    QJsonArray sections;
    for (const ChartSection& section : notation->interaction()->chartSections()) {
        QJsonObject item;
        item["name"] = section.name;
        item["first_measure"] = section.firstMeasure;
        item["measure_count"] = section.measureCount;
        sections.append(item);
    }
    summary["sections"] = sections;

    // the recording the score plays along with, and where measure 1 begins in it
    const kn::playback::ScoreRecording recording = kn::playback::scoreRecording(score->masterScore());
    if (recording.isSet()) {
        QJsonObject item;
        item["path"] = recording.path;
        item["offset_seconds"] = recording.offset;
        summary["recording"] = item;
    }
    // its timing: hidden tempo marks keep every measure where it is in the recording (the tempo marks above
    // are the ones a player reads)
    if (kn::engraving::tempoFollowsRecording(score)) {
        summary["tempo_follows_recording"] = true;
        summary["hidden_tempo_changes"] = kn::engraving::recordingTempoMarkCount(score);
    }

    notation->interaction()->clearSelection();
    summary["drum_tab"] = notation->interaction()->drumTabText(true);

    return QJsonDocument(summary).toJson(QJsonDocument::Indented);
}

//! KitNote: changes a score from the command line - for AI tools (tools/kitnote/mcp, the "kitnote"
//! skill) and for tools/kitnote/smoke_test.py. The commands come from the environment variable
//! KITNOTE_APPLY, separated by ";", or from the file KITNOTE_APPLY_FILE names, one to a line. They are
//! applied after the score is loaded and before it is written ("KitNote -o out.kitnote in.kitnote").
//! A command acts on the whole score, or, after "@<first>-<last> " (or "@<measure> "), on those measures.
//!   pattern:<name>    a groove, fill or rudiment ("rock-8" ... as in the actions "drum-pattern-<name>")
//!   tabpaste:<file>   a drum tab written from the first measure (of the range)
//!   tab:<file>        the drum tab of the score (of the range) written to a file
//!   info:<file>       title, tempo, time signature, measures, sections and drum tab as JSON
//!   title:<text>  composer:<text>  tempo:<quarters a minute>  timesig:<n>/<d>  measures:<count>
//!   (title:, composer:, measures: and swing are for the whole score; "@9 tempo:140" is a tempo change at 9)
//!   @5-8 repeat  @5-8 repeat:<times played>  @8 volta:1  @9-10 volta:2  @5-10 repeat-remove
//!   clear  section:<name>  fillsection:<pattern>  sticking-r  sticking-l  counting  repeats  write-out
//!   key  key-remove  swing-8  swing-16  change:<transform>  step:<pitch>:<step>  stepkind:<pitch>:<step>
//!   recording:<audio file>  the recording to play along with (playback/kitnoterecording.h); where measure 1
//!                           begins in it is found from the notes written so far - give it after them
//!   recordingoffset:<seconds>  where measure 1 begins in it, given      recording-remove
//!   recordingtimes:<file>   where each measure begins in the recording (seconds, the first measure's first, the
//!                           end of the last one last; kitnote_audio.py's measure_times): the playback follows
//!                           them with hidden tempo marks (engraving/editing/recordingtempo.h), and measure 1
//!                           begins at the first - a recording: before it is not looked for in the recording
//!   recordingtempo-remove   the hidden tempo marks of a recording taken away: the score plays its tempo marks
//!   (the recording's path is kept relative to the score written, or to KITNOTE_APPLY_SCORE: where the
//!   caller puts the score afterwards)
//! (The old name of the variable, KITNOTE_TEST_APPLY, is read too.)
//! The most measures the commands may make a score have (an AI tool asking for a billion would keep
//! KitNote busy for good)
static constexpr int KITNOTE_MAX_MEASURES = 2000;

static void applyKitNoteTestCommands(const INotationPtr& notation, const kfw::io::path_t& in, const kfw::io::path_t& out)
{
    QStringList commandList;
    const QString commandFile = qEnvironmentVariable("KITNOTE_APPLY_FILE");
    if (!commandFile.isEmpty()) {
        QFile file(commandFile);
        if (file.open(QIODevice::ReadOnly)) {
            QString text = QString::fromUtf8(file.readAll());
            // a file written by Windows PowerShell 5.1 begins with a byte order mark: not part of the first command
            if (text.startsWith(QChar(0xFEFF))) {
                text.remove(0, 1);
            }
            commandList = text.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
        }
    }
    QString commands = qEnvironmentVariable("KITNOTE_APPLY");
    if (commands.isEmpty()) {
        commands = qEnvironmentVariable("KITNOTE_TEST_APPLY");
    }
    commandList += commands.split(QLatin1Char(';'), Qt::SkipEmptyParts);

    if (commandList.isEmpty() || !notation) {
        return;
    }

    static const std::map<QString, DrumPattern> PATTERNS {
        { QStringLiteral("rock-8"), DrumPattern::Rock8 },
        { QStringLiteral("rock-8-push"), DrumPattern::Rock8Push },
        { QStringLiteral("rock-16"), DrumPattern::Rock16 },
        { QStringLiteral("quarter"), DrumPattern::Quarter },
        { QStringLiteral("four-floor"), DrumPattern::FourOnFloor },
        { QStringLiteral("half-time"), DrumPattern::HalfTime },
        { QStringLiteral("ride-8"), DrumPattern::Ride8 },
        { QStringLiteral("crash-rock-8"), DrumPattern::CrashRock8 },
        { QStringLiteral("snare-fill"), DrumPattern::SnareFill },
        { QStringLiteral("tom-fill"), DrumPattern::TomFill },
        { QStringLiteral("shuffle"), DrumPattern::Shuffle },
        { QStringLiteral("triplet-feel"), DrumPattern::TripletFeel },
        { QStringLiteral("triplet-fill"), DrumPattern::TripletFill },
        { QStringLiteral("disco"), DrumPattern::Disco },
        { QStringLiteral("bossa-nova"), DrumPattern::BossaNova },
        { QStringLiteral("six-eight"), DrumPattern::SixEight },
        { QStringLiteral("half-bar-fill"), DrumPattern::HalfBarFill },
        { QStringLiteral("motown"), DrumPattern::Motown },
        { QStringLiteral("jazz-ride"), DrumPattern::JazzRide },
        { QStringLiteral("reggae"), DrumPattern::ReggaeOneDrop },
        { QStringLiteral("single-stroke"), DrumPattern::RudimentSingleStroke },
        { QStringLiteral("double-stroke"), DrumPattern::RudimentDoubleStroke },
        { QStringLiteral("paradiddle"), DrumPattern::RudimentParadiddle },
        { QStringLiteral("stroke-triplets"), DrumPattern::RudimentStrokeTriplets },
        { QStringLiteral("double-paradiddle"), DrumPattern::RudimentDoubleParadiddle },
        { QStringLiteral("paradiddle-diddle"), DrumPattern::RudimentParadiddleDiddle },
        { QStringLiteral("flams"), DrumPattern::RudimentFlams },
        { QStringLiteral("flam-tap"), DrumPattern::RudimentFlamTap },
        { QStringLiteral("flam-accent"), DrumPattern::RudimentFlamAccent },
    };

    const INotationInteractionPtr interaction = notation->interaction();

    kn::engraving::Score* score = notation->elements()->msScore();

    // what could not be done, for the caller (kitnote-mcp) when it asks with KITNOTE_APPLY_REPORT
    QStringList problems;

    // the times of the recording's measures given: a recording attached is not searched for measure 1
    const bool timesGiven = std::any_of(commandList.cbegin(), commandList.cend(), [](const QString& command) {
        return command.trimmed().startsWith(QLatin1String("recordingtimes:"));
    });

    for (QString command : commandList) {
        command = command.trimmed();
        if (command.isEmpty() || command.startsWith(QLatin1Char('#'))) {
            continue;
        }

        // "@3-4 pattern:rock-8": the measures the command acts on
        bool scoped = false;
        int rangeFirst = 0;     // their numbers, as written
        int rangeLast = 0;
        kn::engraving::Measure* rangeFrom = nullptr;
        kn::engraving::Measure* rangeTo = nullptr;
        interaction->selectAll();
        if (command.startsWith(QLatin1Char('@'))) {
            const QString range = command.section(QLatin1Char(' '), 0, 0).mid(1);
            command = command.section(QLatin1Char(' '), 1).trimmed();
            const int first = range.section(QLatin1Char('-'), 0, 0).toInt();
            const int last = range.contains(QLatin1Char('-')) ? range.section(QLatin1Char('-'), 1, 1).toInt() : first;
            if (first < 1 || last < first) {
                problems << "a range of measures goes from a first to a later one: @" + range;
                continue;
            }
            if (last > KITNOTE_MAX_MEASURES) {
                problems << QString("a score has at most %1 measures: @%2").arg(KITNOTE_MAX_MEASURES).arg(range);
                continue;
            }
            // writing past the end: the score gets the measures it needs
            if (command.startsWith(QLatin1String("pattern:")) || command.startsWith(QLatin1String("tabpaste:"))) {
                int count = 0;
                for (kn::engraving::Measure* measure = score->firstMeasure(); measure; measure = measure->nextMeasure()) {
                    count = std::max(count, measure->no() + 1);
                }
                const int needed = command.startsWith(QLatin1String("pattern:")) ? last : first;
                if (needed > count) {
                    interaction->addBoxes(BoxType::Measure, needed - count, AddBoxesTarget::AtEndOfScore);
                }
            }
            const auto [from, to] = kitNoteMeasures(score, first, last);
            if (!from || !to) {
                LOGE() << "KITNOTE_APPLY: no measures " << range;
                problems << "no measures " + range;
                continue;
            }
            // (a range of ticks would only be found at the next layout: segments are there now)
            score->deselectAll();
            kn::engraving::Measure* after = to->nextMeasure();
            score->selection().setRange(from->first(kn::engraving::SegmentType::ChordRest),
                                        after ? after->first(kn::engraving::SegmentType::ChordRest) : nullptr, 0, score->nstaves());
            score->selection().updateSelectedElements();
            score->setSelectionChanged(true);
            scoped = true;
            rangeFirst = first;
            rangeLast = last;
            rangeFrom = from;
            rangeTo = to;
        }

        // what belongs to the whole score is not put on some measures quietly
        if (scoped && (command.startsWith(QLatin1String("title:")) || command.startsWith(QLatin1String("composer:"))
                       || command.startsWith(QLatin1String("measures:")) || command.startsWith(QLatin1String("swing-"))
                       || command.startsWith(QLatin1String("recording")))) {
            problems << command.section(QLatin1Char(':'), 0, 0) + " applies to the whole score: give it without @measures";
            continue;
        }

        if (command.startsWith(QLatin1String("title:"))) {
            setKitNoteFrameText(notation, kn::engraving::TextStyleType::TITLE, command.mid(6).trimmed());
            continue;
        } else if (command.startsWith(QLatin1String("recordingtimes:")) || command == QLatin1String("recordingtempo-remove")) {
            // the timing of the recording (or no more of it), as one step of the undo history
            kn::engraving::MasterScore* master = score->masterScore();
            std::vector<double> times;
            if (command.startsWith(QLatin1String("recordingtimes:"))) {
                QFile file(command.mid(15).trimmed());
                if (!file.open(QIODevice::ReadOnly)) {
                    problems << "recordingtimes: the file was not found: " + file.fileName();
                    continue;
                }
                // (numbers apart by spaces, lines or commas: a JSON list will do)
                static const QRegularExpression APART(QStringLiteral("[\\s,\\[\\]]+"));
                for (const QString& word : QString::fromUtf8(file.readAll()).split(APART, Qt::SkipEmptyParts)) {
                    bool ok = false;
                    times.push_back(word.toDouble(&ok));
                    if (!ok) {
                        times.clear();
                        break;
                    }
                }
                if (times.size() < 2) {
                    problems << "recordingtimes: needs the times (seconds) where the measures begin, one after another";
                    continue;
                }
            }
            notation->undoStack()->prepareChanges(kfw::TranslatableString("undoableAction", "Follow the timing of the recording"));
            if (times.empty()) {
                kn::engraving::removeRecordingTempo(master);
            } else {
                const kn::engraving::RecordingTempo followed = kn::engraving::followRecordingTimes(master, times);
                if (followed.measures == 0) {
                    problems << "recordingtimes: the times of the measures must go up one after another";
                } else {
                    LOGI() << "KitNote: " << followed.measures << " measures follow the recording with " << followed.hiddenMarks
                           << " hidden tempo marks and " << followed.marks.size() << " tempo changes, within "
                           << followed.largestDrift * 1000.0 << " ms";
                    // measure 1 begins where the recording's first measure begins
                    kn::playback::ScoreRecording recording = kn::playback::scoreRecording(master);
                    if (recording.isSet()) {
                        recording.offset = std::round(times.front() * 1000.0) / 1000.0;
                        master->undo(new kn::engraving::ChangeMetaTags(master, kn::playback::metaTagsWithRecording(master, recording)));
                    }
                }
            }
            notation->undoStack()->commitChanges();
            continue;
        } else if (command.startsWith(QLatin1String("recording:")) || command.startsWith(QLatin1String("recordingoffset:"))
                   || command == QLatin1String("recording-remove")) {
            // the score's recording: its path kept relative to the score written (when that is a score)
            kn::engraving::MasterScore* master = score->masterScore();
            kn::playback::ScoreRecording recording = kn::playback::scoreRecording(master);
            if (command == QLatin1String("recording-remove")) {
                recording = kn::playback::ScoreRecording();
            } else if (command.startsWith(QLatin1String("recordingoffset:"))) {
                bool ok = false;
                const double offset = command.mid(16).trimmed().toDouble(&ok);
                if (!ok || !recording.isSet()) {
                    problems << (recording.isSet() ? "recordingoffset: needs a number of seconds"
                                 : "recordingoffset: the score has no recording (recording:<file> first)");
                    continue;
                }
                recording.offset = offset;
            } else {
                const QString file = QFileInfo(command.mid(10).trimmed()).absoluteFilePath();
                if (!QFileInfo(file).isFile()) {
                    problems << "the recording was not found: " + file;
                    continue;
                }
                // the score's own place: where it is written, or where a caller moves it afterwards (kitnote-mcp)
                const std::string suffix = kfw::io::suffix(out);
                const bool writesScore = suffix == "kitnote" || suffix == "mscz" || suffix == "mscx";
                const QString finalScore = qEnvironmentVariable("KITNOTE_APPLY_SCORE");
                recording.path = kn::playback::recordingPathToStore(
                    file, !finalScore.isEmpty() ? finalScore : (writesScore ? out : in).toQString());
                // (the times of the measures in it, given after this, say where measure 1 begins)
                const std::optional<double> offset = timesGiven ? std::optional<double>(0.0) : kn::playback::findRecordingOffset(
                    file, kn::playback::scoreNotes(notation->masterNotation()->playback(), 1.0), kn::playback::measureOneSecs(master));
                recording.offset = offset ? std::round(*offset * 1000.0) / 1000.0 : 0.0;
                if (!offset) {
                    problems << "the recording is attached, but where measure 1 begins in it was not found: "
                                "give it with recordingoffset:<seconds>";
                }
            }
            notation->undoStack()->prepareChanges(kfw::TranslatableString("undoableAction", "Change the recording"));
            master->undo(new kn::engraving::ChangeMetaTags(master, kn::playback::metaTagsWithRecording(master, recording)));
            notation->undoStack()->commitChanges();
            continue;
        } else if (command.startsWith(QLatin1String("composer:"))) {
            setKitNoteFrameText(notation, kn::engraving::TextStyleType::COMPOSER, command.mid(9).trimmed());
            continue;
        } else if (command.startsWith(QLatin1String("info:"))) {
            QFile file(command.mid(5));
            if (file.open(QIODevice::WriteOnly)) {
                file.write(kitNoteScoreSummary(notation));
            }
            continue;
        } else if (command == QLatin1String("clear")) {
            interaction->deleteSelection();
            continue;
        } else if (command.startsWith(QLatin1String("measures:"))) {
            // as many measures as asked for: added at the end, or taken away from the end
            const int wanted = command.mid(9).toInt();
            if (wanted < 1 || wanted > KITNOTE_MAX_MEASURES) {
                problems << QString("measures: needs a number from 1 to %1").arg(KITNOTE_MAX_MEASURES);
                continue;
            }
            int count = 0;
            for (kn::engraving::Measure* measure = score->firstMeasure(); measure; measure = measure->nextMeasure()) {
                count = std::max(count, measure->no() + 1);
            }
            if (wanted > count) {
                interaction->addBoxes(BoxType::Measure, wanted - count, AddBoxesTarget::AtEndOfScore);
            } else if (wanted < count) {
                const auto [from, to] = kitNoteMeasures(score, wanted + 1, count);
                if (from && to) {
                    // (not interaction->removeSelectedMeasures: it may ask, and nobody is there to answer)
                    score->deselectAll();
                    notation->undoStack()->prepareChanges(kfw::TranslatableString("undoableAction", "Delete measures"));
                    score->deleteMeasures(from, to);
                    notation->undoStack()->commitChanges();
                }
            }
            continue;
        } else if (command.startsWith(QLatin1String("timesig:"))) {
            // from the first measure (of the range) on
            const int numerator = command.mid(8).section(QLatin1Char('/'), 0, 0).toInt();
            const int denominator = command.mid(8).section(QLatin1Char('/'), 1, 1).toInt();
            // measure 1 of the music, past a drum key, when no measures are named
            kn::engraving::Measure* measure = scoped && score->selection().startSegment()
                                              ? score->selection().startSegment()->measure() : kitNoteMeasures(score, 1, 1).first;
            if (numerator < 1 || numerator > 32 || !QList<int> { 1, 2, 4, 8, 16, 32 }.contains(denominator)) {
                problems << "timesig: needs a time signature such as 4/4, 3/4, 6/8 or 7/8";
                continue;
            }
            if (measure) {
                kn::engraving::TimeSig* sig = kn::engraving::Factory::createTimeSig(score->dummy()->segment());
                sig->setSig(kn::engraving::Fraction(numerator, denominator));
                // for every staff (interaction->addTimeSignature makes one of a single staff)
                notation->undoStack()->prepareChanges(kfw::TranslatableString("undoableAction", "Add time signature"));
                score->cmdAddTimeSig(measure, 0, sig, false);
                notation->undoStack()->commitChanges();
            }
            continue;
        }

        if (command.startsWith(QLatin1String("pattern:"))) {
            const auto it = PATTERNS.find(command.mid(8));
            if (it != PATTERNS.cend()) {
                // a pattern that is not written (no edit made) is said, not passed over
                const size_t edits = score->undoStack()->currentIndex();
                interaction->fillSelectionWithDrumPattern(it->second);
                if (score->undoStack()->currentIndex() == edits) {
                    problems << "pattern:" + command.mid(8) + " could not be written there: the kit of the staff lacks a drum "
                                "it uses, or the measures are not on a drum staff";
                }
                continue;
            }
        } else if (command == QLatin1String("sticking-r") || command == QLatin1String("sticking-l")) {
            interaction->addAlternatingSticking(command.endsWith(QLatin1Char('r')));
            continue;
        } else if (command.startsWith(QLatin1String("tabpaste:"))) {
            if (!scoped) {
                interaction->clearSelection();
            }
            QFile file(command.mid(9));
            if (file.open(QIODevice::ReadOnly)) {
                const QString text = QString::fromUtf8(file.readAll());
                // rows whose lines would land in the wrong measures: refused, not written wrongly
                const QString problem = interaction->drumTabProblem(text);
                if (!problem.isEmpty()) {
                    problems << "drum tab: " + problem;
                    continue;
                }
                int skipped = 0;
                const int written = interaction->pasteDrumTabText(text, &skipped);
                if (skipped > 0) {
                    problems << QString("%1 measure(s) of the tab could not be written: every line of a measure needs as many "
                                        "columns as the measure has steps (4/4: 16 sixteenths, 8 eighths or 12 triplet eighths; "
                                        "3/4: 12, 6 or 9 triplet eighths; 6/8: 12 or 6), and no stroke between them").arg(skipped);
                } else if (written == 0) {
                    problems << "the tab has no line KitNote can read (a line is a drum label, then | and the steps)";
                }
            }
            continue;
        } else if (command.startsWith(QLatin1String("measuretab:"))) {
            // "My patterns": the first measure as a pattern (the hook has selected everything)
            QFile file(command.mid(11));
            if (file.open(QIODevice::WriteOnly)) {
                file.write(interaction->selectedMeasureDrumTab().toUtf8());
            }
            continue;
        } else if (command.startsWith(QLatin1String("tabfill:"))) {
            QFile file(command.mid(8));
            if (file.open(QIODevice::ReadOnly)) {
                interaction->fillSelectionWithDrumTab(QString::fromUtf8(file.readAll()));
            }
            continue;
        } else if (command.startsWith(QLatin1String("tab:"))) {
            if (!scoped) {
                interaction->clearSelection();
            }
            QFile file(command.mid(4));
            if (file.open(QIODevice::WriteOnly)) {
                file.write(interaction->drumTabText().toUtf8());
            }
            continue;
        } else if (command.startsWith(QLatin1String("fillsection:"))) {
            const auto it = PATTERNS.find(command.mid(12));
            if (it != PATTERNS.cend()) {
                // (the hook has selected everything: the section is the one the first measure is in)
                interaction->fillChartSection(it->second, true);
                continue;
            }
        } else if (command.startsWith(QLatin1String("spatium:"))) {
            // (Format > Staff size does this through the style)
            // the staff space in millimetres (1.75 by default): a number in a range a page can hold
            bool ok = false;
            const double millimetres = command.mid(8).trimmed().toDouble(&ok);
            if (!ok || millimetres < 0.5 || millimetres > 10) {
                problems << "spatium: needs a staff space in millimetres from 0.5 to 10: " + command;
                continue;
            }
            interaction->clearSelection();
            notation->undoStack()->prepareChanges(kfw::TranslatableString("undoableAction", "Change staff size"));
            notation->style()->setStyleValue(kn::engraving::Sid::spatium, millimetres * kn::engraving::DPMM);
            notation->undoStack()->commitChanges();
            continue;
        } else if (command.startsWith(QLatin1String("tempo:"))) {
            bool ok = false;
            const int tempo = command.mid(6).toInt(&ok);
            if (!ok || tempo < 20 || tempo > 400) {
                problems << "tempo: needs quarter notes a minute, 20 to 400";
                continue;
            }
            if (!scoped) {
                interaction->setMusicTempo(tempo);      // the tempo mark at the first measure
            } else if (rangeLast != rangeFirst) {
                problems << QString("a tempo change goes at one measure and holds from there on: @%1 tempo:%2").arg(rangeFirst).arg(tempo);
            } else {
                interaction->setTempoAtMeasure(tempo, rangeFrom);
            }
            continue;
        } else if (command == QLatin1String("repeat") || command.startsWith(QLatin1String("repeat:"))
                   || command.startsWith(QLatin1String("volta:")) || command == QLatin1String("repeat-remove")) {
            // repeat barlines and endings: "@5-8 repeat", "@5-8 repeat:3", "@8 volta:1", "@9-10 volta:2", "@5-10 repeat-remove"
            const QString name = command.section(QLatin1Char(':'), 0, 0);
            if (!scoped) {
                problems << name + " needs its measures: @5-8 " + command;
                continue;
            }
            if (!rangeTo || rangeTo->no() + 1 != rangeLast) {
                problems << QString("%1: the score has no measure %2").arg(name).arg(rangeLast);
                continue;
            }
            if (command == QLatin1String("repeat-remove")) {
                interaction->removeRepeats(rangeFrom, rangeTo);
                continue;
            }
            QString problem;
            if (name == QLatin1String("volta")) {
                // "1", "2", "1,2" or "1-3": the passes that play these measures
                std::vector<int> endings;
                for (const QString& part : command.mid(6).split(QLatin1Char(','), Qt::SkipEmptyParts)) {
                    bool okFrom = false;
                    bool okTo = false;
                    const int lowest = part.section(QLatin1Char('-'), 0, 0).trimmed().toInt(&okFrom);
                    const int highest = part.contains(QLatin1Char('-'))
                                        ? part.section(QLatin1Char('-'), 1, 1).trimmed().toInt(&okTo) : lowest;
                    if (!okFrom || (part.contains(QLatin1Char('-')) && !okTo) || lowest < 1 || highest < lowest || highest > 10) {
                        endings.clear();
                        break;
                    }
                    for (int ending = lowest; ending <= highest; ++ending) {
                        if (std::find(endings.begin(), endings.end(), ending) == endings.end()) {
                            endings.push_back(ending);
                        }
                    }
                }
                problem = endings.empty() ? QString("volta: needs the endings, 1 to 10: volta:1, volta:2, volta:1,2")
                          : interaction->addVolta(rangeFrom, rangeTo, endings);
            } else {
                bool ok = true;
                const int playCount = command == QLatin1String("repeat") ? 2 : command.mid(7).trimmed().toInt(&ok);
                problem = !ok || playCount < 2 || playCount > 10 ? QString("repeat: the number of times played is 2 to 10: repeat:3")
                          : interaction->addRepeat(rangeFrom, rangeTo, playCount);
            }
            if (!problem.isEmpty()) {
                problems << problem;
            }
            continue;
        } else if (command.startsWith(QLatin1String("section:"))) {
            interaction->addChartSection(command.mid(8));
            continue;
        } else if (command.startsWith(QLatin1String("change:"))) {
            static const std::map<QString, DrumTransform> TRANSFORMS {
        { QStringLiteral("hihat-to-ride"), DrumTransform::HiHatToRide },
        { QStringLiteral("ride-to-hihat"), DrumTransform::RideToHiHat },
        { QStringLiteral("swap-sticking"), DrumTransform::SwapSticking },
        { QStringLiteral("accent-beats"), DrumTransform::AccentBeats },
        { QStringLiteral("accent-offbeats"), DrumTransform::AccentOffBeats },
        { QStringLiteral("accent-clear"), DrumTransform::ClearAccents },
            };
            const auto it = TRANSFORMS.find(command.mid(7));
            if (it != TRANSFORMS.cend()) {
                interaction->transformDrumSelection(it->second);
                continue;
            }
        } else if (command == QLatin1String("steptriplets")) {
            interaction->setDrumStepTriplets(true);
            continue;
        } else if (command.startsWith(QLatin1String("stepkind:"))) {
            const QStringList parts = command.split(QLatin1Char(':'));
            if (parts.size() == 3) {
                interaction->cycleDrumStepKind(parts.at(1).toInt(), parts.at(2).toInt());
                continue;
            }
        } else if (command.startsWith(QLatin1String("step:"))) {
            const QStringList parts = command.split(QLatin1Char(':'));
            if (parts.size() == 3) {
                interaction->toggleDrumStep(parts.at(1).toInt(), parts.at(2).toInt());
                continue;
            }
        } else if (command == QLatin1String("counting")) {
            interaction->addBeatCounting();
            continue;
        } else if (command == QLatin1String("repeats")) {
            interaction->replaceRepeatedMeasuresWithRepeatSigns();
            continue;
        } else if (command == QLatin1String("write-out")) {
            interaction->writeOutMeasureRepeatSigns();
            continue;
        } else if (command == QLatin1String("key-remove")) {
            interaction->removeDrumKey();
            continue;
        } else if (command == QLatin1String("key")) {
            interaction->insertDrumKey();
            continue;
        }

        if (command == QLatin1String("swing-8") || command == QLatin1String("swing-16") || command == QLatin1String("swing-off")) {
            const kn::engraving::DurationType unit = command.endsWith(QLatin1Char('8')) ? kn::engraving::DurationType::V_EIGHTH
                                                     : command.endsWith(QLatin1String("16")) ? kn::engraving::DurationType::V_16TH
                                                     : kn::engraving::DurationType::V_ZERO;
            notation->undoStack()->prepareChanges(kfw::TranslatableString("undoableAction", "Change swing"));
            notation->style()->setStyleValue(kn::engraving::Sid::swingUnit,
                                             kfw::String::fromAscii(kn::engraving::TConv::toXml(unit).ascii()));
            notation->undoStack()->commitChanges();
            continue;
        }

        LOGE() << "KITNOTE_APPLY: unknown command " << command;
        problems << "unknown command: " + command;
    }

    const QString reportFile = qEnvironmentVariable("KITNOTE_APPLY_REPORT");
    if (!reportFile.isEmpty() && !problems.isEmpty()) {
        QFile file(reportFile);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(problems.join('\n').toUtf8());
        }
    }
}


Ret ConverterController::batchConvert(const path_t& batchJobFile, const OpenParams& openParams,
                                      const String& soundProfile,
                                      ProgressPtr progress)
{
    TRACEFUNC;

    if (progress) {
        progress->start();
    }

    RetVal<BatchJob> batchJob = parseBatchJob(batchJobFile);
    if (!batchJob.ret) {
        LOGE() << "failed parse batch job file, err: " << batchJob.ret.toString();
        if (progress) {
            progress->finish(ProgressResult(batchJob.ret));
        }
        return batchJob.ret;
    }

    StringList errors;

    int64_t current = 0;
    int64_t total = batchJob.val.size();
    for (const Job& job : batchJob.val) {
        if (progress) {
            ++current;
            progress->progress(current, total, job.in.toStdString());
        }

        Ret ret = convertFile(job.in, job.out, openParams, soundProfile, job.tracksDiffPath, job.transposeOptions,
                              job.pageNum, job.visibleParts,
                              job.copyright);
        if (!ret) {
            errors.emplace_back(String(u"failed convert, err: %1, in: %2, out: %3")
                                .arg(String::fromStdString(ret.toString())).arg(job.in.toString()).arg(job.out.toString()));
        }
    }

    Ret ret;
    if (!errors.empty()) {
        ret = make_ret(Err::ConvertFailed, errors.join(u"\n").toStdString());
    } else {
        ret = make_ret(Ret::Code::Ok);
    }

    if (progress) {
        progress->finish(ProgressResult(ret));
    }

    return ret;
}

Ret ConverterController::fileConvert(const path_t& in, const path_t& out,
                                     const OpenParams& openParams,
                                     const String& soundProfile,
                                     const path_t& tracksDiffPath,
                                     const std::string& transposeOptionsJson,
                                     const std::optional<ConvertTarget>& target)
{
    std::optional<TransposeOptions> transposeOptions;

    if (!transposeOptionsJson.empty()) {
        RetVal<TransposeOptions> transposeOptionsRet = ConverterUtils::parseTransposeOptions(transposeOptionsJson);
        if (!transposeOptionsRet.ret) {
            return transposeOptionsRet.ret;
        }

        transposeOptions = transposeOptionsRet.val;
    }

    return convertFile(in, out, openParams, soundProfile, tracksDiffPath, transposeOptions, target);
}

Ret ConverterController::convertFile(const kfw::io::path_t& in, const kfw::io::path_t& out,
                                     const OpenParams& openParams,
                                     const String& soundProfile,
                                     const path_t& tracksDiffPath,
                                     const std::optional<notation::TransposeOptions>& transposeOptions,
                                     const std::optional<ConvertTarget>& target,
                                     const std::vector<size_t>& visibleParts,
                                     const CopyrightInfo& copyright)
{
    TRACEFUNC;

    LOGI() << "in: " << in << ", out: " << out;

    std::string suffix = io::suffix(out);

    auto writer = writers()->writer(suffix);
    if (!writer) {
        return make_ret(Err::ConvertTypeUnknown);
    }

    auto notationProject = notationCreator()->newProject(iocContext());
    IF_ASSERT_FAILED(notationProject) {
        return make_ret(Err::UnknownError);
    }

    Ret ret = notationProject->load(in, openParams);
    if (!ret) {
        LOGE() << "failed load notation, err: " << ret.toString() << ", path: " << in;
        return make_ret(Err::InFileFailedLoad);
    }

    QJsonArray oldTracks;
    if (!tracksDiffPath.empty()) {
        oldTracks = NotationMeta::tracksJsonArray(notationProject->masterNotation()->notation());
    }

    if (!soundProfile.isEmpty()) {
        notationProject->audioSettings()->clearTrackInputParams();
        notationProject->audioSettings()->setActiveSoundProfile(soundProfile);
    }

    if (transposeOptions.has_value()) {
        ret = ConverterUtils::applyTranspose(notationProject->masterNotation()->notation(), transposeOptions.value());
        if (!ret) {
            LOGE() << "Failed to apply transposition, err: " << ret.toString();
            return ret;
        }
    }

    applyKitNoteTestCommands(notationProject->masterNotation()->notation(), in, out);

    if (!visibleParts.empty()) {
        ConverterUtils::setVisibleParts(notationProject->masterNotation()->notation(), visibleParts);
    }

    if (!copyright.text.isEmpty()) {
        auto masterScore = notationProject->masterNotation()->masterScore();

        engraving::MStyle& style = masterScore->style();
        String footerOdd = style.value(engraving::Sid::oddFooterC).value<String>();
        String footerEven = style.value(engraving::Sid::evenFooterC).value<String>();
        footerOdd += copyright.text;
        footerEven += copyright.text;
        style.set(engraving::Sid::oddFooterC, footerOdd);
        style.set(engraving::Sid::evenFooterC, footerEven);

        masterScore->doLayout();
    }

    globalContext()->setCurrentProject(notationProject);

    DEFER {
        globalContext()->setCurrentProject(nullptr);
    };

    // Check if this is a part conversion job
    QString baseName = QString::fromStdString(io::completeBasename(out).toStdString());
    if (baseName.contains('*')) {
        return convertScoreParts(writer, notationProject->masterNotation(), out);
    }

    const bool pageNumIsSet = target.has_value() && std::holds_alternative<page_num_t>(target.value());
    const bool regionIsSet = target.has_value() && std::holds_alternative<ConvertRegionJson>(target.value());

    if (suffix == engraving::MSCZ || suffix == engraving::KITNOTE || suffix == engraving::MSCX || suffix == engraving::MSCS) {
        if (pageNumIsSet) {
            return notationProject->savePage(out, std::get<page_num_t>(target.value()));
        } else if (regionIsSet) {
            return saveRegion(notationProject, std::get<ConvertRegionJson>(target.value()), out);
        }

        return notationProject->save(out);
    }

    if (pageNumIsSet || isConvertPageByPage(suffix)) {
        if (pageNumIsSet) {
            page_num_t pageNum = std::get<page_num_t>(target.value());
            ret = convertPage(writer, notationProject->masterNotation()->notation(), pageNum, out);
        } else {
            ret = convertPageByPage(writer, notationProject->masterNotation()->notation(), out);
        }

        if (!ret) {
            LOGE() << "Failed to convert page by page, err: " << ret.toString();
        }
    } else {
        ret = convertFullNotation(writer, notationProject->masterNotation()->notation(), out);
        if (!ret) {
            LOGE() << "Failed to convert full notation, err: " << ret.toString();
        }
    }

    if (ret && !tracksDiffPath.empty()) {
        ret = writeTracksDiff(notationProject, oldTracks, tracksDiffPath);
    }

    return ret;
}

Ret ConverterController::convertScoreParts(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams)
{
    TRACEFUNC;

    auto notationProject = notationCreator()->newProject(iocContext());
    IF_ASSERT_FAILED(notationProject) {
        return make_ret(Err::UnknownError);
    }

    std::string suffix = io::suffix(out);
    auto writer = writers()->writer(suffix);
    if (!writer) {
        return make_ret(Err::ConvertTypeUnknown);
    }

    Ret ret = notationProject->load(in, openParams);
    if (!ret) {
        LOGE() << "failed load notation, err: " << ret.toString() << ", path: " << in;
        return make_ret(Err::InFileFailedLoad);
    }

    ret = convertScoreParts(writer, notationProject->masterNotation(), out);

    return ret;
}

Ret ConverterController::convertScoreParts(INotationWriterPtr writer, IMasterNotationPtr masterNotation, const kfw::io::path_t& out)
{
    std::string suffix = io::suffix(out);

    if (suffix == PDF_SUFFIX) {
        return convertScorePartsToPdf(writer, masterNotation, out);
    } else if (suffix == PNG_SUFFIX) {
        return convertScorePartsToPngs(writer, masterNotation, out);
    } else if (suffix == MP3_SUFFIX) {
        return convertScorePartsToMp3(writer, masterNotation, out);
    }

    return make_ret(Ret::Code::NotSupported);
}

RetVal<ConverterController::BatchJob> ConverterController::parseBatchJob(const kfw::io::path_t& batchJobFile) const
{
    TRACEFUNC;

    RetVal<BatchJob> rv;
    QFile file(batchJobFile.toQString());
    if (!file.open(QIODevice::ReadOnly)) {
        rv.ret = make_ret(Err::BatchJobFileFailedOpen, file.errorString().toStdString());
        return rv;
    }

    const QByteArray data = file.readAll();

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isArray()) {
        rv.ret = make_ret(Err::BatchJobFileFailedParse, err.errorString().toStdString());
        return rv;
    }

    const QJsonArray arr = doc.array();

    auto correctUserInputPath = [](const QString& path) -> QString {
        return io::Dir::fromNativeSeparators(path).toQString();
    };

    for (const auto obj : arr) {
        Job job;
        job.in = correctUserInputPath(obj[u"in"].toString());

        QJsonObject transposeOptionsObj = obj[u"transpose"].toObject();
        if (!transposeOptionsObj.isEmpty()) {
            RetVal<TransposeOptions> transposeOptions = ConverterUtils::parseTransposeOptions(transposeOptionsObj);
            if (!transposeOptions.ret) {
                rv.ret = transposeOptions.ret;
                return rv;
            }
            job.transposeOptions = transposeOptions.val;
        }

        QJsonValue pageVal = obj[u"page"];
        if (!pageVal.isUndefined()) {
            job.pageNum = pageVal.toInt() - 1;
        }

        QJsonValue visibleParts = obj[u"visibleParts"];
        if (!visibleParts.isUndefined()) {
            if (visibleParts.isArray()) {
                const QJsonArray partsArray = visibleParts.toArray();
                for (const auto part : partsArray) {
                    if (part.isDouble()) {
                        job.visibleParts.push_back(part.toInt());
                    } else {
                        LOGE() << "Visible parts value must be a Number";
                    }
                }
            } else {
                rv.ret = make_ret(Err::BatchJobFileFailedParse, err.errorString().toStdString());
                return rv;
            }
        }

        QJsonValue copyright = obj[u"copyright"];
        if (!copyright.isUndefined()) {
            if (copyright.isObject()) {
                QJsonValue copyrightText = copyright[u"text"];
                if (copyrightText.isUndefined()) {
                    rv.ret = make_ret(Err::BatchJobFileFailedParse, err.errorString().toStdString());
                    return rv;
                }

                job.copyright.text = copyrightText.toString();

                QJsonValue showOnAllPages = copyright[u"showOnAllPages"];
                if (!showOnAllPages.isUndefined() && showOnAllPages.isBool()) {
                    job.copyright.showOnAllPages = showOnAllPages.toBool();
                }
            } else {
                rv.ret = make_ret(Err::BatchJobFileFailedParse, err.errorString().toStdString());
                return rv;
            }
        }

        const QJsonValue tracksDiffValue = obj[u"tracksDiff"];
        if (tracksDiffValue.isString()) {
            job.tracksDiffPath = correctUserInputPath(tracksDiffValue.toString());
        }

        const QJsonValue outValue = obj[u"out"];
        if (outValue.isString()) {
            job.out = correctUserInputPath(outValue.toString());
            rv.val.push_back(std::move(job));
        } else if (outValue.isArray()) {
            const QJsonArray outArray = outValue.toArray();
            for (const auto outItem : outArray) {
                Job partJob = job; // Copy the input path
                if (outItem.isString()) {
                    partJob.out = correctUserInputPath(outItem.toString());
                } else if (outItem.isArray() && outItem.toArray().size() == 2) {
                    const QJsonArray partOutArray = outItem.toArray();
                    const QString prefix = correctUserInputPath(partOutArray[0].toString());
                    const QString suffix = partOutArray[1].toString();
                    partJob.out = prefix + "*" + suffix; // Use "*" as a placeholder for part names
                }
                rv.val.push_back(std::move(partJob));
            }
        }
    }

    rv.ret = make_ret(Ret::Code::Ok);
    return rv;
}

bool ConverterController::isConvertPageByPage(const std::string& suffix) const
{
    static const std::unordered_set<std::string> TYPES {
        PNG_SUFFIX,
        SVG_SUFFIX,
    };

    return kfw::contains(TYPES, suffix);
}

Ret ConverterController::convertPageByPage(INotationWriterPtr writer, INotationPtr notation, const kfw::io::path_t& out) const
{
    TRACEFUNC;

    for (size_t i = 0; i < notation->elements()->pages().size(); i++) {
        const String filePath = kfw::io::path_t(io::dirpath(out) + "/"
                                                 + io::completeBasename(out) + "-%1."
                                                 + io::suffix(out)).toString().arg(i + 1);

        Ret ret = convertPage(writer, notation, i, filePath, out);
        if (!ret) {
            return ret;
        }
    }

    return make_ret(Ret::Code::Ok);
}

kfw::Ret ConverterController::convertPage(INotationWriterPtr writer, INotationPtr notation, const size_t pageNum,
                                           const kfw::io::path_t& filePath, const kfw::io::path_t& dirPath) const
{
    TRACEFUNC;

    File file(filePath);
    if (!file.open(File::WriteOnly)) {
        return make_ret(Err::OutFileFailedOpen);
    }

    const INotationWriter::Options options {
        { INotationWriter::OptionKey::PAGE_NUMBER, Val(static_cast<int>(pageNum)) },
    };

    file.setMeta("file_path", filePath.toStdString());

    if (!dirPath.empty()) {
        file.setMeta("dir_path", dirPath.toStdString());
    }

    Ret ret = writer->write(notation, file, options);
    if (!ret) {
        return make_ret(Err::OutFileFailedWrite);
    }

    file.close();

    return make_ok();
}

Ret ConverterController::convertFullNotation(INotationWriterPtr writer, INotationPtr notation, const kfw::io::path_t& out,
                                             const project::INotationWriter::Options& options) const
{
    File file(out);
    if (!file.open(File::WriteOnly)) {
        return make_ret(Err::OutFileFailedOpen);
    }

    file.setMeta("file_path", out.toStdString());
    Ret ret = writer->write(notation, file, options);
    if (!ret) {
        LOGE() << "failed write, err: " << ret.toString() << ", path: " << out;
        return make_ret(Err::OutFileFailedWrite);
    }

    file.close();

    return make_ret(Ret::Code::Ok);
}

Ret ConverterController::convertScorePartsToPdf(INotationWriterPtr writer, IMasterNotationPtr masterNotation,
                                                const kfw::io::path_t& out) const
{
    TRACEFUNC;

    const INotationWriter::Options options {
        { INotationWriter::OptionKey::UNIT_TYPE, Val(INotationWriter::UnitType::PER_PART) },
    };

    for (const IExcerptNotationPtr& e : masterNotation->excerpts()) {
        QString partName = e->notation()->name();
        QString baseName = QString::fromStdString(io::completeBasename(out).toStdString());
        kfw::io::path_t partOut = io::dirpath(out) + "/" + baseName.replace("*", partName).toStdString() + ".pdf";

        File file(partOut);
        if (!file.open(File::WriteOnly)) {
            return make_ret(Err::OutFileFailedOpen);
        }

        Ret ret = writer->write(e->notation(), file, options);
        if (!ret) {
            LOGE() << "failed write, err: " << ret.toString() << ", path: " << partOut;
            return make_ret(Err::OutFileFailedWrite);
        }

        file.close();
    }

    return make_ret(Ret::Code::Ok);
}

Ret ConverterController::convertScorePartsToPngs(INotationWriterPtr writer, kn::notation::IMasterNotationPtr masterNotation,
                                                 const kfw::io::path_t& out) const
{
    TRACEFUNC;

    for (const IExcerptNotationPtr& e : masterNotation->excerpts()) {
        QString partName = e->notation()->name();
        QString baseName = QString::fromStdString(io::completeBasename(out).toStdString());
        kfw::io::path_t pngFilePath = io::dirpath(out) + "/" + baseName.replace("*", partName).toStdString() + ".png";
        Ret ret = convertPageByPage(writer, e->notation(), pngFilePath);
        if (!ret) {
            return ret;
        }
    }

    return make_ret(Ret::Code::Ok);
}

Ret ConverterController::convertScorePartsToMp3(INotationWriterPtr writer, IMasterNotationPtr masterNotation,
                                                const kfw::io::path_t& out) const
{
    TRACEFUNC;

    const INotationWriter::Options options {
        { INotationWriter::OptionKey::UNIT_TYPE, Val(INotationWriter::UnitType::PER_PART) },
    };

    for (const IExcerptNotationPtr& e : masterNotation->excerpts()) {
        QString partName = e->notation()->name();
        QString baseName = QString::fromStdString(io::completeBasename(out).toStdString());
        kfw::io::path_t partOut = io::dirpath(out) + "/" + baseName.replace("*", partName).toStdString() + ".mp3";

        File file(partOut);
        if (!file.open(File::WriteOnly)) {
            return make_ret(Err::OutFileFailedOpen);
        }

        file.setMeta("file_path", partOut.toStdString());
        Ret ret = writer->write(e->notation(), file, options);
        if (!ret) {
            LOGE() << "failed write, err: " << ret.toString() << ", path: " << partOut;
            return make_ret(Err::OutFileFailedWrite);
        }

        file.close();
    }

    return make_ret(Ret::Code::Ok);
}

kfw::Ret ConverterController::saveRegion(INotationProjectPtr project, const ConvertRegionJson& regionJson, const path_t& out) const
{
    RetVal<ConvertRegion> region = ConverterUtils::parseRegion(regionJson);
    if (!region.ret) {
        return region.ret;
    }

    INotationPtr notation = project->masterNotation()->notation();
    const kn::engraving::Score* score = notation->elements()->msScore();

    Measure* startMeasure = score->crMeasure(static_cast<int>(region.val.start.measureIdx));
    if (!startMeasure) {
        startMeasure = score->firstMeasure();
    }

    Measure* endMeasure = score->crMeasure(static_cast<int>(region.val.end.measureIdx));
    if (!endMeasure) {
        endMeasure = score->lastMeasure();
    }

    IF_ASSERT_FAILED(startMeasure && endMeasure) {
        return make_ret(Err::UnknownError);
    }

    INotationInteractionPtr interaction = notation->interaction();

    if (!region.val.voiceIdxSet.empty()) {
        using VoiceFilterType = kn::engraving::VoicesSelectionFilterTypes;
        const std::vector<std::pair<size_t, kn::engraving::VoicesSelectionFilterTypes> > VOICE_FILTERS {
            { 0, VoiceFilterType::FIRST_VOICE },
            { 1, VoiceFilterType::SECOND_VOICE },
            { 2, VoiceFilterType::THIRD_VOICE },
            { 3, VoiceFilterType::FOURTH_VOICE },
        };

        for (const auto& pair : VOICE_FILTERS) {
            const bool voiceAccepted = kfw::contains(region.val.voiceIdxSet, pair.first);
            interaction->selectionFilter()->setSelectionTypeFiltered(pair.second, voiceAccepted);
        }
    }

    interaction->select({ startMeasure }, SelectType::RANGE, region.val.start.staffIdx);
    interaction->select({ endMeasure }, SelectType::RANGE, region.val.end.staffIdx);

    return project->save(out, SaveMode::SaveSelection);
}

Ret ConverterController::writeTracksDiff(INotationProjectPtr project, const QJsonArray& oldTracks, const path_t& path) const
{
    TRACEFUNC;

    File file(path);
    if (!file.open(File::WriteOnly)) {
        return make_ret(Err::TracksDiffFileFailedOpen);
    }

    QJsonObject root;
    root["oldTracks"] = oldTracks;
    root["newTracks"] = NotationMeta::tracksJsonArray(project->masterNotation()->notation());

    QJsonDocument document(root);
    QByteArray qJson = document.toJson(QJsonDocument::Compact);
    ByteArray json = ByteArray::fromQByteArrayNoCopy(qJson);

    return file.write(json);
}

Ret ConverterController::exportScoreMedia(const kfw::io::path_t& in, const kfw::io::path_t& out,
                                          const OpenParams& openParams,
                                          const kfw::io::path_t& highlightConfigPath)
{
    TRACEFUNC;

    return BackendApi::exportScoreMedia(in, out, highlightConfigPath, openParams);
}

Ret ConverterController::exportScoreMeta(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams)
{
    TRACEFUNC;

    return BackendApi::exportScoreMeta(in, out, openParams);
}

Ret ConverterController::exportScoreParts(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams)
{
    TRACEFUNC;

    return BackendApi::exportScoreParts(in, out, openParams);
}

Ret ConverterController::exportScorePartsPdfs(const kfw::io::path_t& in, const kfw::io::path_t& out, const OpenParams& openParams)
{
    TRACEFUNC;

    return BackendApi::exportScorePartsPdfs(in, out, openParams);
}

Ret ConverterController::exportScoreTranspose(const kfw::io::path_t& in, const kfw::io::path_t& out, const std::string& optionsJson,
                                              const OpenParams& openParams)
{
    TRACEFUNC;

    return BackendApi::exportScoreTranspose(in, out, optionsJson, openParams);
}

Ret ConverterController::exportScoreElements(const kfw::io::path_t& in, const kfw::io::path_t& out,
                                             const OpenParams& openParams)
{
    TRACEFUNC;

    return BackendApi::exportScoreElements(in, out, openParams);
}

Ret ConverterController::updateSource(const kfw::io::path_t& in, const std::string& newSource, bool forceMode)
{
    TRACEFUNC;

    return BackendApi::updateSource(in, newSource, forceMode);
}
