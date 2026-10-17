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
#include "abstractstyledialogmodel.h"
#include "iinteractive.h"
#include "notation/inotationconfiguration.h"

namespace kn::notation {
class ChordSymbolsPageModel : public AbstractStyleDialogModel
{
    Q_OBJECT

    Q_PROPERTY(kn::notation::StyleItem * chordStylePreset READ chordStylePreset CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * chordDescriptionFile READ chordDescriptionFile CONSTANT)

    Q_PROPERTY(kn::notation::StyleItem * extensionMag READ extensionMag CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * extensionAdjust READ extensionAdjust CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * modifierMag READ modifierMag CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * modifierAdjust READ modifierAdjust CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * verticallyStackModifiers READ verticallyStackModifiers CONSTANT)

    Q_PROPERTY(kn::notation::StyleItem * chordBassNoteStagger READ chordBassNoteStagger CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * chordBassNoteScale READ chordBassNoteScale CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * polychordDividerThickness READ polychordDividerThickness CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * polychordDividerSpacing READ polychordDividerSpacing CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * useSmuflSymbols READ useSmuflSymbols CONSTANT)

    Q_PROPERTY(kn::notation::StyleItem * verticallyAlignChordSymbols READ verticallyAlignChordSymbols CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * chordAlignmentToNotehead READ chordAlignmentToNotehead CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * chordAlignmentToFretboard READ chordAlignmentToFretboard CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * chordAlignmentExcludeModifiers READ chordAlignmentExcludeModifiers CONSTANT)

    Q_PROPERTY(kn::notation::StyleItem * chordSymbolSpelling READ chordSymbolSpelling CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * automaticCapitalization READ automaticCapitalization CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * lowerCaseMinorChords READ lowerCaseMinorChords CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * lowerCaseBassNotes READ lowerCaseBassNotes CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * allCapsNoteNames READ allCapsNoteNames CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * harmonyFretDist READ harmonyFretDist CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * minHarmonyDist READ minHarmonyDist CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * harmonyHarmonyDist READ harmonyHarmonyDist CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * harmonyVoiceLiteral READ harmonyVoiceLiteral CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * harmonyVoicing READ harmonyVoicing CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * harmonyDuration READ harmonyDuration CONSTANT)

    Q_PROPERTY(kn::notation::StyleItem * displayCapoChords READ displayCapoChords CONSTANT)
    Q_PROPERTY(kn::notation::StyleItem * capoPosition READ capoPosition CONSTANT)

    Q_PROPERTY(bool isCustomXml READ isCustomXml NOTIFY changePreset)
    Q_PROPERTY(bool isLegacyXml READ isLegacyXml NOTIFY changePreset)

    QML_ELEMENT;

    kfw::GlobalInject<kn::notation::INotationConfiguration> configuration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:

    explicit ChordSymbolsPageModel(QObject* parent = nullptr);

    StyleItem* chordStylePreset() const;
    StyleItem* chordDescriptionFile() const;
    Q_INVOKABLE QVariantList possiblePresetOptions() const;
    bool isCustomXml() const;
    bool isLegacyXml() const;

    StyleItem* extensionMag() const;
    StyleItem* extensionAdjust() const;

    StyleItem* modifierMag() const;
    StyleItem* modifierAdjust() const;
    StyleItem* verticallyStackModifiers() const;

    StyleItem* chordBassNoteStagger() const;
    StyleItem* chordBassNoteScale() const;

    StyleItem* polychordDividerThickness() const;
    StyleItem* polychordDividerSpacing() const;

    StyleItem* useSmuflSymbols() const;

    StyleItem* verticallyAlignChordSymbols() const;
    StyleItem* chordAlignmentToNotehead() const;
    StyleItem* chordAlignmentToFretboard() const;
    StyleItem* chordAlignmentExcludeModifiers() const;

    StyleItem* chordSymbolSpelling() const;
    Q_INVOKABLE QVariantList possibleChordSymbolSpellings() const;

    StyleItem* automaticCapitalization() const;
    StyleItem* lowerCaseMinorChords() const;
    StyleItem* lowerCaseBassNotes() const;
    StyleItem* allCapsNoteNames() const;

    StyleItem* harmonyFretDist() const;
    StyleItem* minHarmonyDist() const;
    StyleItem* harmonyHarmonyDist() const;

    Q_INVOKABLE QVariantList possibleHarmonyVoiceLiteralOptions() const;
    Q_INVOKABLE QVariantList possibleHarmonyVoicingOptions() const;
    Q_INVOKABLE QVariantList possibleHarmonyDurationOptions() const;

    StyleItem* harmonyVoiceLiteral() const;
    StyleItem* harmonyVoicing() const;
    StyleItem* harmonyDuration() const;

    StyleItem* displayCapoChords() const;
    Q_INVOKABLE QVariantList possibleCapoDisplayOptions() const;
    StyleItem* capoPosition() const;

signals:
    void changePreset();

public slots:
    void setChordStyle(kn::engraving::ChordStylePreset);
    void selectChordDescriptionFile();
};
}
