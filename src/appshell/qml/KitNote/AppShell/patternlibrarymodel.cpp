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

#include "patternlibrarymodel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>

#include "notation/inotation.h"
#include "notation/inotationinteraction.h"
#include "notation/notationtypes.h"

#include "settings.h"

using namespace kn::appshell;
using namespace kn::notation;

namespace {
struct LibraryPattern {
    const char* code;
    DrumPattern pattern;
};

const std::vector<std::vector<LibraryPattern> > LIBRARY {
    {
        { "drum-pattern-rock-8", DrumPattern::Rock8 },
        { "drum-pattern-rock-8-push", DrumPattern::Rock8Push },
        { "drum-pattern-rock-16", DrumPattern::Rock16 },
        { "drum-pattern-quarter", DrumPattern::Quarter },
        { "drum-pattern-crash-rock-8", DrumPattern::CrashRock8 },
        { "drum-pattern-ride-8", DrumPattern::Ride8 },
        { "drum-pattern-half-time", DrumPattern::HalfTime },
        { "drum-pattern-four-floor", DrumPattern::FourOnFloor },
        { "drum-pattern-disco", DrumPattern::Disco },
        { "drum-pattern-motown", DrumPattern::Motown },
        { "drum-pattern-bossa-nova", DrumPattern::BossaNova },
        { "drum-pattern-reggae", DrumPattern::ReggaeOneDrop },
        { "drum-pattern-shuffle", DrumPattern::Shuffle },
        { "drum-pattern-triplet-feel", DrumPattern::TripletFeel },
        { "drum-pattern-jazz-ride", DrumPattern::JazzRide },
        { "drum-pattern-six-eight", DrumPattern::SixEight },
    },
    {
        { "drum-pattern-snare-fill", DrumPattern::SnareFill },
        { "drum-pattern-tom-fill", DrumPattern::TomFill },
        { "drum-pattern-half-bar-fill", DrumPattern::HalfBarFill },
        { "drum-pattern-triplet-fill", DrumPattern::TripletFill },
    },
    {
        { "drum-rudiment-single-stroke", DrumPattern::RudimentSingleStroke },
        { "drum-rudiment-double-stroke", DrumPattern::RudimentDoubleStroke },
        { "drum-rudiment-paradiddle", DrumPattern::RudimentParadiddle },
        { "drum-rudiment-stroke-triplets", DrumPattern::RudimentStrokeTriplets },
        { "drum-rudiment-double-paradiddle", DrumPattern::RudimentDoubleParadiddle },
        { "drum-rudiment-paradiddle-diddle", DrumPattern::RudimentParadiddleDiddle },
        { "drum-rudiment-flams", DrumPattern::RudimentFlams },
        { "drum-rudiment-flam-tap", DrumPattern::RudimentFlamTap },
        { "drum-rudiment-flam-accent", DrumPattern::RudimentFlamAccent },
    },
};
}

//! [ { "name": ..., "tab": "HH|x-x-x-x-x-x-x-x-|\nSD|...|\n" } ]
static const kfw::Settings::Key USER_PATTERNS_KEY("appshell", "ui/kitnote/userPatterns");
//! what the setting held when it could not be read, kept before the first pattern is saved over it
static const kfw::Settings::Key USER_PATTERNS_BACKUP_KEY("appshell", "ui/kitnote/userPatternsUnreadable");

//! the patterns saved before; "readable" false when the setting holds something that is not a list of them
static QJsonArray readUserPatterns(bool* readable = nullptr)
{
    const std::string json = kfw::settings()->value(USER_PATTERNS_KEY).toString();
    const QJsonDocument document = QJsonDocument::fromJson(QByteArray::fromStdString(json));
    if (readable) {
        *readable = json.empty() || document.isArray();
    }
    return document.array();
}

static void writeUserPatterns(const QJsonArray& patterns)
{
    const QByteArray json = QJsonDocument(patterns).toJson(QJsonDocument::Compact);
    kfw::settings()->setSharedValue(USER_PATTERNS_KEY, kfw::Val(json.toStdString()));
}

//! The rows of a one-measure drum tab, in the form of the library patterns.
static QVariantMap userPatternItem(const QString& name, const QString& tab)
{
    static const QMap<QString, int> FAMILIES {
        { "BD", 1 }, { "SD", 2 }, { "HH", 3 }, { "HF", 3 }, { "CC", 4 }, { "RD", 4 }, { "T1", 5 }, { "T2", 5 }, { "FT", 5 }
    };

    QVariantList rows;
    int steps = 0;
    int beats = 4;
    for (const QString& line : tab.split(QLatin1Char('\n'), Qt::SkipEmptyParts)) {
        if (line.startsWith(QLatin1Char('@'))) {
            // "@6/8": two beats of three eighths; "@3/4": three beats
            const QStringList parts = line.mid(1).split(QLatin1Char('/'));
            const int numerator = parts.value(0).toInt();
            const int denominator = parts.value(1).toInt();
            if (numerator > 0) {
                beats = denominator == 8 && numerator % 3 == 0 ? numerator / 3 : numerator;
            }
            continue;
        }
        const qsizetype bar = line.indexOf(QLatin1Char('|'));
        const qsizetype end = line.indexOf(QLatin1Char('|'), bar + 1);
        if (bar < 0 || end < 0) {
            continue;
        }

        QVariantList states;
        for (const QChar& sign : line.mid(bar + 1, end - bar - 1)) {
            // as the library's previews: 0 nothing, 1 a stroke, 2 an accent, 3 a ghost note
            states << (sign == QLatin1Char('-') ? 0 : sign.isUpper() ? 2 : sign == QLatin1Char('g') ? 3 : 1);
        }
        steps = static_cast<int>(states.size());

        const QString label = line.left(bar).trimmed();
        QVariantMap row;
        row["label"] = label;
        row["family"] = FAMILIES.value(label, 0);
        row["states"] = states;
        rows << row;
    }

    QVariantMap item;
    item["title"] = name;
    item["steps"] = steps;
    item["group"] = steps > 0 && steps % beats == 0 ? steps / beats : std::max(steps, 1);
    item["sticking"] = QString();
    item["rows"] = rows;
    item["user"] = true;
    return item;
}

QVariantList PatternLibraryModel::userPatterns() const
{
    QVariantList result;
    for (const QJsonValue& value : readUserPatterns()) {
        const QJsonObject pattern = value.toObject();
        result << userPatternItem(pattern.value("name").toString(), pattern.value("tab").toString());
    }
    return result;
}

bool PatternLibraryModel::saveSelectedMeasure(const QString& name)
{
    const kn::notation::INotationPtr notation = globalContext()->currentNotation();
    const QString tab = notation ? notation->interaction()->selectedMeasureDrumTab() : QString();
    if (tab.isEmpty()) {
        return false;
    }

    bool readable = true;
    QJsonArray patterns = readUserPatterns(&readable);
    if (!readable) {
        // the saved patterns cannot be read (a hand-edited settings file, say): kept aside, not written over
        kfw::settings()->setSharedValue(USER_PATTERNS_BACKUP_KEY, kfw::settings()->value(USER_PATTERNS_KEY));
    }

    QString title = name.trimmed();
    if (title.isEmpty()) {
        title = kfw::qtrc("appshell", "My pattern %1").arg(patterns.size() + 1);
    }

    QJsonObject pattern;
    pattern["name"] = title;
    pattern["tab"] = tab;
    patterns.append(pattern);
    writeUserPatterns(patterns);

    emit userPatternsChanged();
    return true;
}

int PatternLibraryModel::applyUserPattern(int index)
{
    const QJsonArray patterns = readUserPatterns();
    const kn::notation::INotationPtr notation = globalContext()->currentNotation();
    if (notation && index >= 0 && index < patterns.size()) {
        return notation->interaction()->fillSelectionWithDrumTab(patterns.at(index).toObject().value("tab").toString());
    }
    return 0;
}

void PatternLibraryModel::removeUserPattern(int index)
{
    QJsonArray patterns = readUserPatterns();
    if (index < 0 || index >= patterns.size()) {
        return;
    }

    patterns.removeAt(index);
    writeUserPatterns(patterns);
    emit userPatternsChanged();
}

//! A fill as a one-measure drum tab in 4/4. It goes down the kit - snare, high tom, mid tom, floor tom -
//! never back up; a beat is one of a few sixteenth-note figures, on one drum or split over two; the first
//! stroke of a beat may be accented, and the bass drum may mark the beats underneath.
QString PatternLibraryModel::randomFillTab(bool halfMeasure, quint32 seed)
{
    QRandomGenerator random(seed);

    static const QStringList FIGURES { "oooo", "oooo", "oo-o", "o-oo", "ooo-", "o-o-" };
    static const QStringList DRUMS { "SD", "T1", "T2", "FT" };

    QMap<QString, QString> lines;
    for (const QString& label : QStringList { "HH", "SD", "T1", "T2", "FT", "BD" }) {
        lines[label] = QString(16, QLatin1Char('-'));
    }

    int firstBeat = 0;
    if (halfMeasure) {
        // two beats of the rock beat first
        lines["HH"].replace(0, 8, QStringLiteral("x-x-x-x-"));
        lines["SD"][4] = QLatin1Char('o');
        lines["BD"][0] = QLatin1Char('o');
        firstBeat = 2;
    }

    const bool kickOnBeats = random.bounded(3) == 0;
    const bool accents = random.bounded(2) == 0;
    int drum = random.bounded(2) == 0 ? 0 : random.bounded(2);      // mostly from the snare

    for (int beat = firstBeat; beat < 4; ++beat) {
        const QString figure = FIGURES.at(random.bounded(static_cast<int>(FIGURES.size())));
        // the second half of the beat may already be on the next drum
        const int second = random.bounded(3) == 0 ? std::min(drum + 1, static_cast<int>(DRUMS.size()) - 1) : drum;

        for (int i = 0; i < 4; ++i) {
            if (figure.at(i) != QLatin1Char('o')) {
                continue;
            }
            const QString& label = DRUMS.at(i < 2 ? drum : second);
            lines[label][beat * 4 + i] = QLatin1Char(accents && i == 0 ? 'O' : 'o');
        }

        if (kickOnBeats) {
            lines["BD"][beat * 4] = QLatin1Char('o');
        }

        // on to the next beat: stay, or go down one or two drums
        drum = std::min(std::max(second, drum + static_cast<int>(random.bounded(3)) - (beat == firstBeat ? 1 : 0)),
                        static_cast<int>(DRUMS.size()) - 1);
    }

    QString tab;
    for (const QString& label : QStringList { "HH", "T1", "T2", "SD", "FT", "BD" }) {
        if (lines[label].count(QLatin1Char('-')) < 16) {
            tab += label + QLatin1Char('|') + lines[label] + QLatin1String("|\n");
        }
    }
    return tab;
}

int PatternLibraryModel::writeRandomFill(bool halfMeasure)
{
    const kn::notation::INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        return notation->interaction()->fillSelectionWithDrumTab(randomFillTab(halfMeasure, QRandomGenerator::global()->generate()));
    }
    return 0;
}

PatternLibraryModel::PatternLibraryModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void PatternLibraryModel::load()
{
    auto watch = [this]() {
        const kn::notation::INotationPtr notation = globalContext()->currentNotation();
        if (notation) {
            notation->notationChanged().onNotify(this, [this]() {
                updateScoreEmpty();
            }, kfw::async::Asyncable::Mode::SetReplace);
        }

        emit scoreOpened();
        updateScoreEmpty();
    };

    globalContext()->currentNotationChanged().onNotify(this, watch);
    watch();
}

void PatternLibraryModel::setWatchEmptyScore(bool watch)
{
    if (m_watchEmptyScore == watch) {
        return;
    }

    m_watchEmptyScore = watch;
    emit watchEmptyScoreChanged();
    updateScoreEmpty();
}

void PatternLibraryModel::updateScoreEmpty()
{
    bool empty = false;
    if (m_watchEmptyScore) {
        const kn::notation::INotationPtr notation = globalContext()->currentNotation();
        empty = notation && notation->interaction()->drumTabText(true).isEmpty();
    }

    if (m_scoreEmpty != empty) {
        m_scoreEmpty = empty;
        emit scoreEmptyChanged();
    }
}

QVariantList PatternLibraryModel::patterns(int group) const
{
    QVariantList result;
    if (group < 0 || group >= static_cast<int>(LIBRARY.size())) {
        return result;
    }

    for (const LibraryPattern& entry : LIBRARY[group]) {
        const kfw::ui::UiAction& action = actionsRegister()->action(entry.code);
        if (!action.isValid()) {
            continue;
        }

        const DrumPatternPreview preview = drumPatternPreview(entry.pattern);

        QVariantList rows;
        for (const DrumPatternPreview::Row& row : preview.rows) {
            QVariantList states;
            for (int state : row.states) {
                states << state;
            }

            QVariantMap item;
            item["label"] = row.label;
            item["family"] = row.family;
            item["states"] = states;
            rows << item;
        }

        QVariantMap item;
        item["code"] = QString::fromLatin1(entry.code);
        item["title"] = action.title.qTranslatedWithoutMnemonic();
        if (group == 1) {
            // "Fill: snare sixteenths" - under the heading "Fills" the first word says nothing
            const QString title = item["title"].toString();
            const qsizetype colon = title.indexOf(QLatin1String(": "));
            if (colon > 0) {
                item["title"] = title.mid(colon + 2, 1).toUpper() + title.mid(colon + 3);
            }
        }
        item["steps"] = preview.steps;
        item["group"] = preview.group;
        item["sticking"] = preview.sticking;
        item["rows"] = rows;
        result << item;
    }

    return result;
}
