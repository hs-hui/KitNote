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
#include "elementrepositoryservice.h"

#include "qml/KitNote/Inspector/types/texttypes.h"

#include "engraving/dom/barline.h"
#include "engraving/dom/beam.h"
#include "engraving/dom/bracket.h"
#include "engraving/dom/bracketItem.h"
#include "engraving/dom/chord.h"
#include "engraving/dom/durationtype.h"
#include "engraving/dom/glissando.h"
#include "engraving/dom/hairpin.h"
#include "engraving/dom/hook.h"
#include "engraving/dom/layoutbreak.h"
#include "engraving/dom/lyrics.h"
#include "engraving/dom/engravingglobals.h"
#include "engraving/dom/note.h"
#include "engraving/dom/note.h"
#include "engraving/dom/pedal.h"
#include "engraving/dom/playcounttext.h"
#include "engraving/dom/segment.h"
#include "engraving/dom/staff.h"
#include "engraving/dom/stafftype.h"
#include "engraving/dom/stem.h"
#include "engraving/dom/text.h"
#include "engraving/dom/trill.h"
#include "engraving/dom/volta.h"

#include "log.h"

using namespace kn::inspector;
using namespace kn::engraving;

bool ElementRepositoryService::needUpdateElementList(const QList<EngravingItem*>& newRawElementList,
                                                     SelState selectionState) const
{
    return m_rawElementList != newRawElementList || m_selectionState != selectionState;
}

void ElementRepositoryService::updateElementList(const QList<EngravingItem*>& newRawElementList,
                                                 SelState selectionState)
{
    if (!needUpdateElementList(newRawElementList, selectionState)) {
        return;
    }

    m_exposedElementList = exposeRawElements(newRawElementList);
    m_rawElementList = newRawElementList;
    m_selectionState = selectionState;

    m_elementsUpdated.send(m_rawElementList);
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findElementsByType(const kn::engraving::ElementType elementType) const
{
    switch (elementType) {
    case kn::engraving::ElementType::CHORD: return findChords();
    case kn::engraving::ElementType::NOTE: return findNotes();
    case kn::engraving::ElementType::NOTEHEAD: return findNoteHeads();
    case kn::engraving::ElementType::STEM: return findStems();
    case kn::engraving::ElementType::HOOK: return findHooks();
    case kn::engraving::ElementType::BEAM: return findBeams();
    case kn::engraving::ElementType::STAFF: return findStaffs();
    case kn::engraving::ElementType::LAYOUT_BREAK: return findSectionBreaks(); //Page breaks and line breaks are of type LAYOUT_BREAK, but they don't appear in the inspector for now.
    case kn::engraving::ElementType::TEXT: return findTexts();
    case kn::engraving::ElementType::BRACKET: return findBrackets();
    case kn::engraving::ElementType::REST: return findRests();
    case kn::engraving::ElementType::ORNAMENT: return findOrnaments();
    case kn::engraving::ElementType::LYRICS: return findLyrics();
    case kn::engraving::ElementType::PEDAL:
    case kn::engraving::ElementType::GLISSANDO:
    case kn::engraving::ElementType::VIBRATO:
    case kn::engraving::ElementType::HAIRPIN:
    case kn::engraving::ElementType::VOLTA:
    case kn::engraving::ElementType::LET_RING:
    case kn::engraving::ElementType::OTTAVA:
    case kn::engraving::ElementType::TEXTLINE:
    case kn::engraving::ElementType::TEXTLINE_BASE:
    case kn::engraving::ElementType::NOTELINE:
    case kn::engraving::ElementType::SLUR:
    case kn::engraving::ElementType::HAMMER_ON_PULL_OFF:
    case kn::engraving::ElementType::TIE:
    case kn::engraving::ElementType::LAISSEZ_VIB:
    case kn::engraving::ElementType::PARTIAL_TIE:
    case kn::engraving::ElementType::GRADUAL_TEMPO_CHANGE:
    case kn::engraving::ElementType::LYRICSLINE:
    case kn::engraving::ElementType::PARTIAL_LYRICSLINE:
    case kn::engraving::ElementType::WHAMMY_BAR:
    case kn::engraving::ElementType::PALM_MUTE:
        return findLines(elementType);
    default:
        QList<kn::engraving::EngravingItem*> resultList;

        for (kn::engraving::EngravingItem* element : m_exposedElementList) {
            if (element->type() == elementType) {
                resultList << element;
            }
        }

        return resultList;
    }
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findElementsByType(const kn::engraving::ElementType elementType,
                                                                                  std::function<bool(const kn::engraving::EngravingItem*)> filterFunc)
const
{
    QList<kn::engraving::EngravingItem*> resultList;

    QList<kn::engraving::EngravingItem*> unfilteredList = findElementsByType(elementType);

    for (kn::engraving::EngravingItem* element : unfilteredList) {
        if (filterFunc(element)) {
            resultList << element;
        }
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::takeAllElements() const
{
    return m_exposedElementList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::exposeRawElements(const QList<kn::engraving::EngravingItem*>& rawElementList)
const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (kn::engraving::EngravingItem* element : rawElementList) {
        kn::engraving::ElementType elementType = element->type();

        //! NOTE: instrument names can't survive the layout process,
        //! so we have to exclude them from the list to prevent
        //! crashes on invalid pointers in the inspector
        if (elementType == kn::engraving::ElementType::INSTRUMENT_NAME) {
            continue;
        }

        if (elementType == kn::engraving::ElementType::BRACKET) {
            resultList << kn::engraving::toBracket(element)->bracketItem();
            continue;
        }

        resultList << element;
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findChords() const
{
    QSet<kn::engraving::EngravingItem*> elements;

    for (kn::engraving::EngravingItem* element : m_exposedElementList) {
        if (element->type() == kn::engraving::ElementType::CHORD) {
            elements << element;
            continue;
        }

        if (element->type() == kn::engraving::ElementType::BEAM) {
            const kn::engraving::Beam* beam = kn::engraving::toBeam(element);

            for (kn::engraving::ChordRest* chordRest : beam->elements()) {
                if (!chordRest->isChord()) {
                    continue;
                }
                elements << chordRest;
            }
            continue;
        }

        kn::engraving::EngravingItem* chord = element->findAncestor(kn::engraving::ElementType::CHORD);
        if (chord) {
            elements << chord;
        }
    }

    return QList<kn::engraving::EngravingItem*>(elements.begin(), elements.end());
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findNotes() const
{
    QList<kn::engraving::EngravingItem*> result;

    for (engraving::EngravingItem* element : m_rawElementList) {
        if (element->isNote()) {
            result << element;
            continue;
        }

        engraving::EngravingItem* elementBase = element->elementBase();

        if (elementBase->isChord()) {
            engraving::Chord* chord = engraving::toChord(elementBase);

            for (kn::engraving::Note* note : chord->notes()) {
                result << note;
            }
        } else if (elementBase->isBeam()) {
            const kn::engraving::Beam* beam = kn::engraving::toBeam(elementBase);

            for (kn::engraving::ChordRest* chordRest : beam->elements()) {
                if (!chordRest->isChord()) {
                    continue;
                }

                for (kn::engraving::Note* note : engraving::toChord(chordRest)->notes()) {
                    result << note;
                }
            }
        }
    }

    return result;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findNoteHeads() const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (kn::engraving::EngravingItem* element : m_rawElementList) {
        if (element->isNote()) {
            resultList << element;
        }
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findStems() const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (const kn::engraving::EngravingItem* element : findChords()) {
        const kn::engraving::Chord* chord = kn::engraving::toChord(element);

        if (chord && chord->stem()) {
            resultList << chord->stem();
        }
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findHooks() const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (const kn::engraving::EngravingItem* element : findChords()) {
        const kn::engraving::Chord* chord = kn::engraving::toChord(element);

        if (chord && chord->hook()) {
            resultList << chord->hook();
        }
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findBeams() const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (const kn::engraving::EngravingItem* element : findChords()) {
        kn::engraving::EngravingItem* beam = nullptr;

        if (element->isChord()) {
            const kn::engraving::Chord* chord = kn::engraving::toChord(element);

            if (!chord) {
                continue;
            }

            beam = chord->beam();
        } else if (element->isBeam()) {
            beam = const_cast<kn::engraving::EngravingItem*>(element);
        }

        if (!beam || resultList.contains(beam)) {
            continue;
        }

        resultList << beam;
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findLines(kn::engraving::ElementType lineType) const
{
    static const QMap<kn::engraving::ElementType, kn::engraving::ElementType> lineTypeToSegmentType {
        { kn::engraving::ElementType::GLISSANDO, kn::engraving::ElementType::GLISSANDO_SEGMENT },
        { kn::engraving::ElementType::VIBRATO, kn::engraving::ElementType::VIBRATO_SEGMENT },
        { kn::engraving::ElementType::PEDAL, kn::engraving::ElementType::PEDAL_SEGMENT },
        { kn::engraving::ElementType::HAIRPIN, kn::engraving::ElementType::HAIRPIN_SEGMENT },
        { kn::engraving::ElementType::VOLTA, kn::engraving::ElementType::VOLTA_SEGMENT },
        { kn::engraving::ElementType::LET_RING, kn::engraving::ElementType::LET_RING_SEGMENT },
        { kn::engraving::ElementType::PALM_MUTE, kn::engraving::ElementType::PALM_MUTE_SEGMENT },
        { kn::engraving::ElementType::OTTAVA, kn::engraving::ElementType::OTTAVA_SEGMENT },
        { kn::engraving::ElementType::TEXTLINE, kn::engraving::ElementType::TEXTLINE_SEGMENT },
        { kn::engraving::ElementType::TEXTLINE_BASE, kn::engraving::ElementType::TEXTLINE_SEGMENT },
        { kn::engraving::ElementType::NOTELINE, kn::engraving::ElementType::NOTELINE_SEGMENT },
        { kn::engraving::ElementType::SLUR, kn::engraving::ElementType::SLUR_SEGMENT },
        { kn::engraving::ElementType::HAMMER_ON_PULL_OFF, kn::engraving::ElementType::HAMMER_ON_PULL_OFF_SEGMENT },
        { kn::engraving::ElementType::TIE, kn::engraving::ElementType::TIE_SEGMENT },
        { kn::engraving::ElementType::LAISSEZ_VIB, kn::engraving::ElementType::LAISSEZ_VIB_SEGMENT },
        { kn::engraving::ElementType::PARTIAL_TIE, kn::engraving::ElementType::PARTIAL_TIE_SEGMENT },
        { kn::engraving::ElementType::GRADUAL_TEMPO_CHANGE, kn::engraving::ElementType::GRADUAL_TEMPO_CHANGE_SEGMENT },
        { kn::engraving::ElementType::LYRICSLINE, kn::engraving::ElementType::LYRICSLINE_SEGMENT },
        { kn::engraving::ElementType::PARTIAL_LYRICSLINE, kn::engraving::ElementType::PARTIAL_LYRICSLINE_SEGMENT }
    };

    QList<kn::engraving::EngravingItem*> resultList;

    IF_ASSERT_FAILED(lineTypeToSegmentType.contains(lineType)) {
        return resultList;
    }

    kn::engraving::ElementType segmentType = lineTypeToSegmentType[lineType];

    for (kn::engraving::EngravingItem* element : m_exposedElementList) {
        if (element->type() == segmentType || (lineType == ElementType::TEXTLINE_BASE && element->isTextLineBaseSegment())) {
            const kn::engraving::SpannerSegment* segment = kn::engraving::toSpannerSegment(element);
            kn::engraving::Spanner* line = segment ? segment->spanner() : nullptr;

            if (line) {
                resultList << line;
            }
        } else if (element->type() == lineType) {
            resultList << element;
        }
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findStaffs() const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (const kn::engraving::EngravingItem* element : m_exposedElementList) {
        if (!element->staff()) {
            continue;
        }

        resultList << element->staff();
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findSectionBreaks() const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (kn::engraving::EngravingItem* element : m_exposedElementList) {
        if (element && element->type() == kn::engraving::ElementType::LAYOUT_BREAK) {
            const kn::engraving::LayoutBreak* layoutBreak = kn::engraving::toLayoutBreak(element);
            if (layoutBreak->layoutBreakType() != kn::engraving::LayoutBreakType::SECTION) {
                continue;
            }

            resultList << element;
        }
    }

    return resultList;
}

std::vector<EngravingItem*> ElementRepositoryService::findTextDelegates(EngravingItem* element) const
{
    if (element->isTextLineBaseSegment()) {
        std::vector<EngravingItem*> textItems;
        TextLineBase* tl = toTextLineBaseSegment(element)->textLineBase();
        for (SpannerSegment* seg : tl->spannerSegments()) {
            if (Text* text = toTextLineBaseSegment(seg)->text()) {
                textItems.push_back(text);
            }

            if (Text* endText = toTextLineBaseSegment(seg)->endText()) {
                textItems.push_back(endText);
            }
        }

        return textItems;
    }

    if (element->isBarLine()) {
        Segment* seg = toBarLine(element)->segment();
        if (PlayCountText* playCountText = toPlayCountText(seg->findAnnotation(ElementType::PLAY_COUNT_TEXT, 0, 0))) {
            return { playCountText };
        }
        return {};
    }

    if (TEXT_ELEMENT_TYPES.contains(element->type())) {
        return { element };
    }

    return {};
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findTexts() const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (kn::engraving::EngravingItem* element : m_exposedElementList) {
        std::vector<EngravingItem*> delegateItems = findTextDelegates(element);
        for (kn::engraving::EngravingItem* el : delegateItems) {
            resultList << el;
        }
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findBrackets() const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (kn::engraving::EngravingItem* element : m_exposedElementList) {
        if (element->isBracketItem()) {
            resultList << element;
        }
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findRests() const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (kn::engraving::EngravingItem* element : m_exposedElementList) {
        if (element->isRest()) {
            resultList << element;
        } else if (element->isBeam()) {
            const kn::engraving::Beam* beam = kn::engraving::toBeam(element);

            for (kn::engraving::ChordRest* chordRest : beam->elements()) {
                if (!chordRest->isRest()) {
                    continue;
                }
                resultList << chordRest;
            }
        }
    }

    return resultList;
}

QList<kn::engraving::EngravingItem*> ElementRepositoryService::findOrnaments() const
{
    QList<kn::engraving::EngravingItem*> resultList;

    for (kn::engraving::EngravingItem* element : m_exposedElementList) {
        if (element->isOrnament()) {
            resultList << element;
        } else if (element->isTrill()) {
            resultList << (EngravingItem*)(toTrill(element)->ornament());
        } else if (element->isTrillSegment()) {
            resultList << (EngravingItem*)(toTrillSegment(element)->trill()->ornament());
        }
    }

    return resultList;
}

QList<EngravingItem*> ElementRepositoryService::findLyrics() const
{
    QList<kn::engraving::EngravingItem*> resultList;
    for (kn::engraving::EngravingItem* element : m_exposedElementList) {
        if (element->isLyrics()) {
            resultList << element;
        }
    }

    return resultList;
}

kfw::async::Channel<QList<EngravingItem*> > ElementRepositoryService::elementsUpdated() const
{
    return m_elementsUpdated;
}
