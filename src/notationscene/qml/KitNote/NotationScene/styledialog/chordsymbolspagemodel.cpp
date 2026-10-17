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

#include "chordsymbolspagemodel.h"
#include "engraving/types/types.h"

using namespace kn::notation;

ChordSymbolsPageModel::ChordSymbolsPageModel(QObject* parent)
    : AbstractStyleDialogModel(parent, {
    StyleId::chordStyle,
    StyleId::chordsXmlFile,
    StyleId::chordDescriptionFile,
    StyleId::chordExtensionMag,
    StyleId::chordExtensionAdjust,
    StyleId::chordModifierMag,
    StyleId::chordModifierAdjust,
    StyleId::verticallyStackModifiers,
    StyleId::chordBassNoteStagger,
    StyleId::chordBassNoteScale,
    StyleId::polychordDividerThickness,
    StyleId::polychordDividerSpacing,
    StyleId::harmonyParenUseSmuflSym,
    StyleId::verticallyAlignChordSymbols,
    StyleId::chordSymPosition,
    StyleId::chordAlignmentToFretboard,
    StyleId::chordAlignmentExcludeModifiers,
    StyleId::chordSymbolSpelling,
    StyleId::automaticCapitalization,
    StyleId::lowerCaseMinorChords,
    StyleId::lowerCaseBassNotes,
    StyleId::allCapsNoteNames,
    StyleId::harmonyFretDist,
    StyleId::minHarmonyDistance,
    StyleId::harmonyHarmonyDistance,
    StyleId::harmonyVoiceLiteral,
    StyleId::harmonyVoicing,
    StyleId::harmonyDuration,
    StyleId::displayCapoChords,
    StyleId::capoPosition,
})
{
}

StyleItem* ChordSymbolsPageModel::chordStylePreset() const { return styleItem(StyleId::chordStyle); }
StyleItem* ChordSymbolsPageModel::chordDescriptionFile() const { return styleItem(StyleId::chordDescriptionFile); }

QVariantList ChordSymbolsPageModel::possiblePresetOptions() const
{
    QVariantList options {
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Standard") },
            { "value", kn::engraving::ChordStylePreset::STANDARD } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Jazz") },
            { "value", kn::engraving::ChordStylePreset::JAZZ } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Legacy MuseScore") },
            { "value", kn::engraving::ChordStylePreset::LEGACY } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Load custom XML…") },
            { "value", kn::engraving::ChordStylePreset::CUSTOM } },
    };

    return options;
}

bool ChordSymbolsPageModel::isCustomXml() const
{
    return chordStylePreset()->value() == kn::engraving::ChordStylePreset::CUSTOM;
}

bool ChordSymbolsPageModel::isLegacyXml() const
{
    return chordStylePreset()->value() == kn::engraving::ChordStylePreset::LEGACY;
}

void ChordSymbolsPageModel::setChordStyle(kn::engraving::ChordStylePreset selection)
{
    switch (selection) {
    case kn::engraving::ChordStylePreset::STANDARD:
        chordDescriptionFile()->modifyValue("chords_std.xml");
        break;
    case kn::engraving::ChordStylePreset::JAZZ:
        chordDescriptionFile()->modifyValue("chords_jazz.xml");
        break;
    case kn::engraving::ChordStylePreset::LEGACY:
        chordBassNoteStagger()->modifyValue(false);
        chordDescriptionFile()->modifyValue("chords_legacy.xml");
        break;
    case kn::engraving::ChordStylePreset::CUSTOM:
        // Handled on file open
        break;
    }

    emit changePreset();
}

void ChordSymbolsPageModel::selectChordDescriptionFile()
{
    kfw::io::path_t dir = configuration()->userStylesPath();
    std::vector<std::string> filter = { kfw::trc("notation", "MuseScore chord symbol style files") + " (*.xml)" };

    kfw::io::path_t path = interactive()->selectOpeningFileSync(kfw::trc("notation", "Load style"), dir, filter);
    if (path.empty()) {
        return;
    }

    chordDescriptionFile()->modifyValue(path.toQString());
}

StyleItem* ChordSymbolsPageModel::extensionMag() const { return styleItem(StyleId::chordExtensionMag); }
StyleItem* ChordSymbolsPageModel::extensionAdjust() const { return styleItem(StyleId::chordExtensionAdjust); }

StyleItem* ChordSymbolsPageModel::modifierMag() const { return styleItem(StyleId::chordModifierMag); }
StyleItem* ChordSymbolsPageModel::modifierAdjust() const { return styleItem(StyleId::chordModifierAdjust); }
StyleItem* ChordSymbolsPageModel::verticallyStackModifiers() const { return styleItem(StyleId::verticallyStackModifiers); }

StyleItem* ChordSymbolsPageModel::chordBassNoteStagger() const { return styleItem(StyleId::chordBassNoteStagger); }
StyleItem* ChordSymbolsPageModel::chordBassNoteScale() const { return styleItem(StyleId::chordBassNoteScale); }

StyleItem* ChordSymbolsPageModel::polychordDividerThickness() const { return styleItem(StyleId::polychordDividerThickness); }
StyleItem* ChordSymbolsPageModel::polychordDividerSpacing() const { return styleItem(StyleId::polychordDividerSpacing); }
StyleItem* kn::notation::ChordSymbolsPageModel::useSmuflSymbols() const { return styleItem(StyleId::harmonyParenUseSmuflSym); }

StyleItem* ChordSymbolsPageModel::verticallyAlignChordSymbols() const { return styleItem(StyleId::verticallyAlignChordSymbols); }
StyleItem* ChordSymbolsPageModel::chordAlignmentToNotehead() const { return styleItem(StyleId::chordSymPosition); }
StyleItem* ChordSymbolsPageModel::chordAlignmentToFretboard() const { return styleItem(StyleId::chordAlignmentToFretboard); }
StyleItem* ChordSymbolsPageModel::chordAlignmentExcludeModifiers() const { return styleItem(StyleId::chordAlignmentExcludeModifiers); }

StyleItem* ChordSymbolsPageModel::chordSymbolSpelling() const { return styleItem(StyleId::chordSymbolSpelling); }
QVariantList ChordSymbolsPageModel::possibleChordSymbolSpellings() const
{
    QVariantList options {
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Standard") },
            { "value", static_cast<int>(engraving::NoteSpellingType::STANDARD) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "German") },
            { "value", static_cast<int>(engraving::NoteSpellingType::GERMAN) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Full German") },
            { "value", static_cast<int>(engraving::NoteSpellingType::GERMAN_PURE) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Solfeggio") },
            { "value", static_cast<int>(engraving::NoteSpellingType::SOLFEGGIO) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "French") },
            { "value", static_cast<int>(engraving::NoteSpellingType::FRENCH) } },
    };

    return options;
}

StyleItem* ChordSymbolsPageModel::automaticCapitalization() const { return styleItem(StyleId::automaticCapitalization); }
StyleItem* ChordSymbolsPageModel::lowerCaseMinorChords() const { return styleItem(StyleId::lowerCaseMinorChords); }
StyleItem* ChordSymbolsPageModel::lowerCaseBassNotes() const { return styleItem(StyleId::lowerCaseBassNotes); }
StyleItem* ChordSymbolsPageModel::allCapsNoteNames() const { return styleItem(StyleId::allCapsNoteNames); }

StyleItem* ChordSymbolsPageModel::harmonyFretDist() const { return styleItem(StyleId::harmonyFretDist); }
StyleItem* ChordSymbolsPageModel::minHarmonyDist() const { return styleItem(StyleId::minHarmonyDistance); }
StyleItem* ChordSymbolsPageModel::harmonyHarmonyDist() const { return styleItem(StyleId::harmonyHarmonyDistance); }

QVariantList ChordSymbolsPageModel::possibleHarmonyVoiceLiteralOptions() const
{
    QVariantList options {
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Jazz") },
            { "value", static_cast<int>(false) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Literal") },
            { "value", static_cast<int>(true) } },
    };

    return options;
}

QVariantList ChordSymbolsPageModel::possibleHarmonyVoicingOptions() const
{
    QVariantList options {
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Automatic") },
            { "value", static_cast<int>(Voicing::AUTO) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Root only") },
            { "value", static_cast<int>(Voicing::ROOT_ONLY) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Close") },
            { "value", static_cast<int>(Voicing::CLOSE) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Drop two") },
            { "value", static_cast<int>(Voicing::DROP_2) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Six note") },
            { "value", static_cast<int>(Voicing::SIX_NOTE) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Four note") },
            { "value", static_cast<int>(Voicing::FOUR_NOTE) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Three note") },
            { "value", static_cast<int>(Voicing::THREE_NOTE) } },
    };

    return options;
}

QVariantList ChordSymbolsPageModel::possibleHarmonyDurationOptions() const
{
    QVariantList options {
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Until next chord symbol") },
            { "value", static_cast<int>(kn::engraving::HDuration::UNTIL_NEXT_CHORD_SYMBOL) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Until end of measure") },
            { "value", static_cast<int>(kn::engraving::HDuration::STOP_AT_MEASURE_END) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Chord/rest duration") },
            { "value", static_cast<int>(kn::engraving::HDuration::SEGMENT_DURATION) } },
    };

    return options;
}

StyleItem* ChordSymbolsPageModel::harmonyVoiceLiteral() const { return styleItem(StyleId::harmonyVoiceLiteral); }
StyleItem* ChordSymbolsPageModel::harmonyVoicing() const { return styleItem(StyleId::harmonyVoicing); }
StyleItem* ChordSymbolsPageModel::harmonyDuration() const { return styleItem(StyleId::harmonyDuration); }

StyleItem* ChordSymbolsPageModel::displayCapoChords() const { return styleItem(StyleId::displayCapoChords); }

QVariantList ChordSymbolsPageModel::possibleCapoDisplayOptions() const
{
    QVariantList options {
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Don’t show chord symbols transposed by capo") },
            { "value", static_cast<int>(kn::engraving::DisplayCapoChordType::CONCERT) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Show chord symbols & transposed symbols") },
            { "value", static_cast<int>(kn::engraving::DisplayCapoChordType::BOTH) } },
        QVariantMap{
            { "text", kfw::qtrc("notation/editstyle/chordsymbols", "Show transposed symbols only") },
            { "value", static_cast<int>(kn::engraving::DisplayCapoChordType::TRANSPOSED) } },
    };

    return options;
}

StyleItem* ChordSymbolsPageModel::capoPosition() const { return styleItem(StyleId::capoPosition); }
