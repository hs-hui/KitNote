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

#include "songstructuremodel.h"

#include "notation/inotation.h"
#include "notation/inotationinteraction.h"
#include "notation/notationtypes.h"
#include "notation/inotationelements.h"
#include "engraving/dom/masterscore.h"
#include "engraving/dom/measure.h"

using namespace kn::appshell;
using namespace kn::notation;

SongStructureModel::SongStructureModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void SongStructureModel::load()
{
    globalContext()->currentNotationChanged().onNotify(this, [this]() {
        onNotationChanged();
    });

    onNotationChanged();
}

void SongStructureModel::setActive(bool active)
{
    if (m_active == active) {
        return;
    }

    m_active = active;
    emit activeChanged();

    update();
}

void SongStructureModel::onNotationChanged()
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        notation->interaction()->selectionChanged().onNotify(this, [this]() {
            update();
        }, kfw::async::Asyncable::Mode::SetReplace);

        notation->notationChanged().onNotify(this, [this]() {
            update();
        }, kfw::async::Asyncable::Mode::SetReplace);
    }

    update();
}

void SongStructureModel::update()
{
    if (!m_active) {
        return;
    }

    QVariantList sections;

    const INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        for (const ChartSection& section : notation->interaction()->chartSections()) {
            QVariantMap item;
            item["name"] = section.name;
            item["firstMeasure"] = section.firstMeasure;
            item["measureCount"] = section.measureCount;
            item["current"] = section.current;
            sections << item;
        }
    }

    if (sections != m_sections) {
        m_sections = sections;
        emit sectionsChanged();
    }
}

void SongStructureModel::goTo(int index)
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        notation->interaction()->goToChartSection(index);
    }
}

QVariantList SongStructureModel::grooves() const
{
    // the names are those of the pattern commands, so that they read the same everywhere
    static const std::vector<std::pair<std::string, DrumPattern> > GROOVES {
        { "drum-pattern-rock-8", DrumPattern::Rock8 },
        { "drum-pattern-rock-16", DrumPattern::Rock16 },
        { "drum-pattern-half-time", DrumPattern::HalfTime },
        { "drum-pattern-four-floor", DrumPattern::FourOnFloor },
        { "drum-pattern-ride-8", DrumPattern::Ride8 },
        { "drum-pattern-shuffle", DrumPattern::Shuffle },
        { "drum-pattern-jazz-ride", DrumPattern::JazzRide },
        { "drum-pattern-disco", DrumPattern::Disco },
        { "drum-pattern-bossa-nova", DrumPattern::BossaNova },
        { "drum-pattern-reggae", DrumPattern::ReggaeOneDrop },
    };

    QVariantList result;
    for (const auto& [code, pattern] : GROOVES) {
        const kfw::ui::UiAction& action = actionsRegister()->action(code);
        if (!action.isValid()) {
            continue;
        }

        QVariantMap item;
        item["title"] = action.title.qTranslatedWithoutMnemonic();
        item["pattern"] = static_cast<int>(pattern);
        result << item;
    }
    return result;
}

void SongStructureModel::fillCurrentSection(int pattern, bool fillAtEnd)
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        notation->interaction()->fillChartSection(static_cast<DrumPattern>(pattern), fillAtEnd);
    }
}

void SongStructureModel::setMusicTempo(int quartersPerMinute)
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        // the taps are beats: in 6/8, 9/8 and 12/8 dotted quarters, one and a half quarters each
        const kn::engraving::Score* score = notation->elements()->msScore();
        const kn::engraving::Measure* first = score ? score->firstMeasure() : nullptr;
        const kn::engraving::Fraction signature = first ? first->timesig() : kn::engraving::Fraction(4, 4);
        const bool compound = signature.denominator() == 8 && signature.numerator() % 3 == 0 && signature.numerator() > 3;
        notation->interaction()->setMusicTempo(compound ? static_cast<int>(std::lround(quartersPerMinute * 1.5)) : quartersPerMinute);
    }
}

void SongStructureModel::addSection(const QString& name)
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        notation->interaction()->addChartSection(name);
    }
}
