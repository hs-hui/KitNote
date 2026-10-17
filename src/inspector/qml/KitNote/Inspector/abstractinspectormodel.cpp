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
#include "abstractinspectormodel.h"

#include "engraving/dom/barline.h"
#include "engraving/dom/dynamic.h"
#include "engraving/dom/property.h"
#include "engraving/dom/tempotext.h"

#include "modularity/ioc.h"
#include "shortcuts/shortcutstypes.h"

#include "types/texttypes.h"

#include "log.h"

using namespace kn::inspector;
using namespace kn::notation;
using namespace kn::engraving;

// KitNote: no sections for what only pitched instruments use (glissandi, vibrato, guitar and keyboard markings,
// key signatures, accidentals, chord symbols, fretboards, ambitus, bends, ornaments, ottavas, string tunings):
// such elements, when an imported score has them, show no Properties section.
static const QMap<kn::engraving::ElementType, InspectorModelType> NOTATION_ELEMENT_MODEL_TYPES = {
    { kn::engraving::ElementType::NOTE, InspectorModelType::TYPE_NOTE },
    { kn::engraving::ElementType::STEM, InspectorModelType::TYPE_NOTE },
    { kn::engraving::ElementType::NOTEDOT, InspectorModelType::TYPE_NOTE },
    { kn::engraving::ElementType::NOTEHEAD, InspectorModelType::TYPE_NOTE },
    { kn::engraving::ElementType::NOTELINE, InspectorModelType::TYPE_NOTE },
    { kn::engraving::ElementType::SHADOW_NOTE, InspectorModelType::TYPE_NOTE },
    { kn::engraving::ElementType::HOOK, InspectorModelType::TYPE_NOTE },
    { kn::engraving::ElementType::BEAM, InspectorModelType::TYPE_NOTE },
    { kn::engraving::ElementType::SLUR, InspectorModelType::TYPE_SLUR },
    { kn::engraving::ElementType::SLUR_SEGMENT, InspectorModelType::TYPE_SLUR },
    { kn::engraving::ElementType::TIE, InspectorModelType::TYPE_TIE },
    { kn::engraving::ElementType::TIE_SEGMENT, InspectorModelType::TYPE_TIE },
    { kn::engraving::ElementType::LAISSEZ_VIB, InspectorModelType::TYPE_LAISSEZ_VIB },
    { kn::engraving::ElementType::LAISSEZ_VIB_SEGMENT, InspectorModelType::TYPE_LAISSEZ_VIB },
    { kn::engraving::ElementType::PARTIAL_TIE, InspectorModelType::TYPE_PARTIAL_TIE },
    { kn::engraving::ElementType::PARTIAL_TIE_SEGMENT, InspectorModelType::TYPE_PARTIAL_TIE },
    { kn::engraving::ElementType::TEMPO_TEXT, InspectorModelType::TYPE_TEMPO },
    { kn::engraving::ElementType::FERMATA, InspectorModelType::TYPE_FERMATA },
    { kn::engraving::ElementType::LAYOUT_BREAK, InspectorModelType::TYPE_SECTIONBREAK },
    { kn::engraving::ElementType::BAR_LINE, InspectorModelType::TYPE_BARLINE },
    { kn::engraving::ElementType::PLAY_COUNT_TEXT, InspectorModelType::TYPE_PLAY_COUNT_TEXT },
    { kn::engraving::ElementType::MARKER, InspectorModelType::TYPE_MARKER },
    { kn::engraving::ElementType::JUMP, InspectorModelType::TYPE_JUMP },
    { kn::engraving::ElementType::SPACER, InspectorModelType::TYPE_SPACER },
    { kn::engraving::ElementType::CLEF, InspectorModelType::TYPE_CLEF },
    { kn::engraving::ElementType::HAIRPIN, InspectorModelType::TYPE_HAIRPIN },
    { kn::engraving::ElementType::HAIRPIN_SEGMENT, InspectorModelType::TYPE_HAIRPIN },
    { kn::engraving::ElementType::VOLTA, InspectorModelType::TYPE_VOLTA },
    { kn::engraving::ElementType::VOLTA_SEGMENT, InspectorModelType::TYPE_VOLTA },
    { kn::engraving::ElementType::STAFFTYPE_CHANGE, InspectorModelType::TYPE_STAFF_TYPE_CHANGES },
    { kn::engraving::ElementType::TBOX, InspectorModelType::TYPE_TEXT_FRAME },// text frame
    { kn::engraving::ElementType::VBOX, InspectorModelType::TYPE_VERTICAL_FRAME },// vertical frame
    { kn::engraving::ElementType::HBOX, InspectorModelType::TYPE_HORIZONTAL_FRAME },// horizontal frame
    { kn::engraving::ElementType::ARTICULATION, InspectorModelType::TYPE_ARTICULATION },
    { kn::engraving::ElementType::IMAGE, InspectorModelType::TYPE_IMAGE },
    { kn::engraving::ElementType::BRACKET, InspectorModelType::TYPE_BRACKET },
    { kn::engraving::ElementType::TIMESIG, InspectorModelType::TYPE_TIME_SIGNATURE },
    { kn::engraving::ElementType::MMREST, InspectorModelType::TYPE_MMREST },
    { kn::engraving::ElementType::TREMOLO_SINGLECHORD, InspectorModelType::TYPE_TREMOLO },
    { kn::engraving::ElementType::TREMOLO_TWOCHORD, InspectorModelType::TYPE_TREMOLO },
    { kn::engraving::ElementType::MEASURE_REPEAT, InspectorModelType::TYPE_MEASURE_REPEAT },
    { kn::engraving::ElementType::TUPLET, InspectorModelType::TYPE_TUPLET },
    { kn::engraving::ElementType::TEXTLINE, InspectorModelType::TYPE_TEXT_LINE },
    { kn::engraving::ElementType::TEXTLINE_SEGMENT, InspectorModelType::TYPE_TEXT_LINE },
    { kn::engraving::ElementType::LET_RING, InspectorModelType::TYPE_TEXT_LINE },
    { kn::engraving::ElementType::LET_RING_SEGMENT, InspectorModelType::TYPE_TEXT_LINE },
    { kn::engraving::ElementType::NOTELINE, InspectorModelType::TYPE_NOTELINE },
    { kn::engraving::ElementType::NOTELINE_SEGMENT, InspectorModelType::TYPE_NOTELINE },
    { kn::engraving::ElementType::GRADUAL_TEMPO_CHANGE, InspectorModelType::TYPE_GRADUAL_TEMPO_CHANGE },
    { kn::engraving::ElementType::GRADUAL_TEMPO_CHANGE_SEGMENT, InspectorModelType::TYPE_GRADUAL_TEMPO_CHANGE },
    { kn::engraving::ElementType::INSTRUMENT_NAME, InspectorModelType::TYPE_INSTRUMENT_NAME },
    { kn::engraving::ElementType::LYRICS, InspectorModelType::TYPE_LYRICS },
    { kn::engraving::ElementType::LYRICSLINE, InspectorModelType::TYPE_LYRICS_LINE },
    { kn::engraving::ElementType::LYRICSLINE_SEGMENT, InspectorModelType::TYPE_LYRICS_LINE },
    { kn::engraving::ElementType::PARTIAL_LYRICSLINE, InspectorModelType::TYPE_PARTIAL_LYRICS_LINE },
    { kn::engraving::ElementType::PARTIAL_LYRICSLINE_SEGMENT, InspectorModelType::TYPE_PARTIAL_LYRICS_LINE },
    { kn::engraving::ElementType::REST, InspectorModelType::TYPE_REST },
    { kn::engraving::ElementType::DYNAMIC, InspectorModelType::TYPE_DYNAMIC },
    { kn::engraving::ElementType::EXPRESSION, InspectorModelType::TYPE_EXPRESSION },
    { kn::engraving::ElementType::SYMBOL, InspectorModelType::TYPE_SYMBOL },
    { kn::engraving::ElementType::SYSTEM_DIVIDER, InspectorModelType::TYPE_SYMBOL },
    { kn::engraving::ElementType::CHORD_BRACKET, InspectorModelType::TYPE_CHORD_BRACKET },
};

static const QMap<kn::engraving::HairpinType, InspectorModelType> HAIRPIN_ELEMENT_MODEL_TYPES = {
    { kn::engraving::HairpinType::CRESC_HAIRPIN, InspectorModelType::TYPE_HAIRPIN },
    { kn::engraving::HairpinType::DIM_HAIRPIN, InspectorModelType::TYPE_HAIRPIN },
    { kn::engraving::HairpinType::CRESC_LINE, InspectorModelType::TYPE_CRESCENDO },
    { kn::engraving::HairpinType::DIM_LINE, InspectorModelType::TYPE_DIMINUENDO },
};

static const QMap<kn::engraving::LayoutBreakType, InspectorModelType> LAYOUT_BREAK_ELEMENT_MODEL_TYPES = {
    { kn::engraving::LayoutBreakType::SECTION, InspectorModelType::TYPE_SECTIONBREAK }
};

static const QMap<kn::engraving::TempoTextType, InspectorModelType> TEMPO_TEXT_ELEMENT_MODEL_TYPES = {
    { kn::engraving::TempoTextType::NORMAL, InspectorModelType::TYPE_TEMPO },
    { kn::engraving::TempoTextType::A_TEMPO, InspectorModelType::TYPE_A_TEMPO },
    { kn::engraving::TempoTextType::TEMPO_PRIMO, InspectorModelType::TYPE_TEMPO_PRIMO },
};

QString AbstractInspectorModel::shortcutsForActionCode(std::string code) const
{
    std::vector<std::string> shortcuts = shortcutsRegister()->shortcut(code).sequences;
    return kfw::shortcuts::sequencesToNativeText(shortcuts);
}

AbstractInspectorModel::AbstractInspectorModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                               IElementRepositoryService* repository,
                                               kn::engraving::ElementType elementType)
    : QObject(parent), kfw::Contextable(iocCtx), m_repository(repository), m_elementType(elementType)
{
    if (!m_repository) {
        return;
    }

    m_repository->elementsUpdated().onReceive(this, [this](const QList<kn::engraving::EngravingItem*>&) {
        updateProperties();
    });

    connect(this, &AbstractInspectorModel::requestReloadPropertyItems, this, &AbstractInspectorModel::updateProperties);
}

void AbstractInspectorModel::init()
{
    onCurrentNotationChanged();
}

bool AbstractInspectorModel::isSystemObjectBelowBottomStaff() const
{
    return m_isSystemObjectBelowBottomStaff;
}

MeasurementUnits AbstractInspectorModel::measurementUnits() const
{
    return m_measurementUnits;
}

bool AbstractInspectorModel::shouldUpdateOnScoreChange() const
{
    return m_shouldUpdateOnScoreChange;
}

bool AbstractInspectorModel::shouldUpdateWhenEmpty() const
{
    return false;
}

bool AbstractInspectorModel::shouldUpdateOnEmptyPropertyAndStyleIdSets() const
{
    return false;
}

void AbstractInspectorModel::updateIsSystemObjectBelowBottomStaff()
{
    bool soBelowBottomStaff = false;
    for (EngravingItem* item : m_elementList) {
        if (item->isSystemObjectBelowBottomStaff()) {
            soBelowBottomStaff = true;
            break;
        }
    }

    if (m_isSystemObjectBelowBottomStaff != soBelowBottomStaff) {
        m_isSystemObjectBelowBottomStaff = soBelowBottomStaff;
        emit isSystemObjectBelowBottomStaffChanged(m_isSystemObjectBelowBottomStaff);
    }
}

void AbstractInspectorModel::updatemeasurementUnits()
{
    MeasurementUnits result = MeasurementUnits::UNITS_UNKNOWN;

    for (EngravingItem* item : m_elementList) {
        MeasurementUnits itemUnits = item->offsetIsSpatiumDependent() ? MeasurementUnits::UNITS_SPATIUM : MeasurementUnits::UNITS_MM;
        if (result != MeasurementUnits::UNITS_UNKNOWN && itemUnits != result) {
            result = MeasurementUnits::UNITS_UNKNOWN;
            break;
        } else {
            result = itemUnits;
        }
    }

    if (m_measurementUnits != result) {
        m_measurementUnits = result;
        emit measurementUnitsChanged(m_measurementUnits);
    }
}

void AbstractInspectorModel::requestResetToDefaults()
{
    resetProperties();
}

QString AbstractInspectorModel::title() const
{
    return m_title;
}

int AbstractInspectorModel::icon() const
{
    return static_cast<int>(m_icon);
}

InspectorSectionType AbstractInspectorModel::sectionType() const
{
    return m_sectionType;
}

InspectorModelType AbstractInspectorModel::modelType() const
{
    return m_modelType;
}

ElementKey AbstractInspectorModel::makeKey(const EngravingItem* item)
{
    switch (item->type()) {
    case ElementType::TEMPO_TEXT: {
        const auto tempoText = toTempoText(item);
        return ElementKey{ ElementType::TEMPO_TEXT, static_cast<int>(tempoText->tempoTextType()) };
    }
    default:
        return ElementKey{ item->type(), item->subtype() };
    }
}

InspectorModelType AbstractInspectorModel::modelTypeByElementKey(const ElementKey& elementKey)
{
    if (elementKey.type == kn::engraving::ElementType::HAIRPIN || elementKey.type == kn::engraving::ElementType::HAIRPIN_SEGMENT) {
        return HAIRPIN_ELEMENT_MODEL_TYPES.value(static_cast<kn::engraving::HairpinType>(elementKey.subtype),
                                                 InspectorModelType::TYPE_UNDEFINED);
    }

    if (elementKey.type == kn::engraving::ElementType::LAYOUT_BREAK) {
        return LAYOUT_BREAK_ELEMENT_MODEL_TYPES.value(static_cast<kn::engraving::LayoutBreakType>(elementKey.subtype),
                                                      InspectorModelType::TYPE_UNDEFINED);
    }

    if (elementKey.type == kn::engraving::ElementType::TEMPO_TEXT) {
        return TEMPO_TEXT_ELEMENT_MODEL_TYPES.value(static_cast<kn::engraving::TempoTextType>(elementKey.subtype),
                                                    InspectorModelType::TYPE_UNDEFINED);
    }

    return NOTATION_ELEMENT_MODEL_TYPES.value(elementKey.type, InspectorModelType::TYPE_UNDEFINED);
}

InspectorModelTypeSet AbstractInspectorModel::modelTypesByElementKeys(const ElementKeySet& elementKeySet)
{
    InspectorModelTypeSet types;

    for (const ElementKey& key : elementKeySet) {
        types << modelTypeByElementKey(key);
    }

    return types;
}

static bool isPureDynamics(const QList<kn::engraving::EngravingItem*>& selectedElementList)
{
    if (selectedElementList.empty()) {
        return false;
    }

    for (const EngravingItem* item : selectedElementList) {
        if (!item->isTextBase()) {
            continue;
        }

        if (!item->isDynamic()) {
            return false;
        }

        const Dynamic* dynamic = toDynamic(item);
        if (dynamic->hasCustomText()) {
            return false;
        }
    }

    return true;
}

static bool barlineWithPlayText(const QList<kn::engraving::EngravingItem*>& selectedElementList)
{
    if (selectedElementList.empty()) {
        return false;
    }

    for (const EngravingItem* item : selectedElementList) {
        if (!item->isBarLine()) {
            continue;
        }

        Segment* seg = toBarLine(item)->segment();
        PlayCountText* playCountText = toPlayCountText(seg->findAnnotation(ElementType::PLAY_COUNT_TEXT, 0, 0));

        if (playCountText) {
            return true;
        }
    }

    return false;
}

static bool hasValidTextLineBaseSegment(const QList<kn::engraving::EngravingItem*>& selectedElementList)
{
    if (selectedElementList.empty()) {
        return false;
    }

    for (const EngravingItem* item : selectedElementList) {
        if (!item->isTextLineBaseSegment()) {
            continue;
        }
        const TextLineBaseSegment* tlbs = toTextLineBaseSegment(item);
        const TextLineBase* tlb = tlbs ? tlbs->textLineBase() : nullptr;
        if (!tlb) {
            continue;
        }
        if (!tlb->beginText().empty() || !tlb->continueText().empty() || !tlb->endText().empty()) {
            return true;
        }
    }

    return false;
}

InspectorSectionTypeSet AbstractInspectorModel::sectionTypesByElementKeys(const ElementKeySet& elementKeySet, bool isRange,
                                                                          const QList<kn::engraving::EngravingItem*>& selectedElementList)
{
    InspectorSectionTypeSet types;

    for (const ElementKey& key : elementKeySet) {
        if (NOTATION_ELEMENT_MODEL_TYPES.contains(key.type)
            && (modelTypeByElementKey(key) != InspectorModelType::TYPE_UNDEFINED)) {
            types << InspectorSectionType::SECTION_NOTATION;
        }

        // Don't show the "Text" inspector panel for "pure" dynamics (i.e. without custom text)
        if ((TEXT_ELEMENT_TYPES.contains(key.type) && !isPureDynamics(selectedElementList)) || barlineWithPlayText(selectedElementList)) {
            types << InspectorSectionType::SECTION_TEXT;
        }

        // Look for a TextLineBaseSegment with begin, continue, or end text...
        if (hasValidTextLineBaseSegment(selectedElementList)) {
            types << InspectorSectionType::SECTION_TEXT;
        }

        if (key.type != kn::engraving::ElementType::INSTRUMENT_NAME) {
            types << InspectorSectionType::SECTION_GENERAL;
        }
    }

    if (isRange) {
        types << InspectorSectionType::SECTION_MEASURES;
        types << InspectorSectionType::SECTION_EMPTY_STAVES;
    }

    if (showPartsSection(selectedElementList)) {
        types << InspectorSectionType::SECTION_PARTS;
    }

    return types;
}

bool AbstractInspectorModel::showPartsSection(const QList<EngravingItem*>& selectedElementList)
{
    static const std::unordered_set<ElementType> noAvailableChangePartsSettingsTypes {
        ElementType::LAYOUT_BREAK,
        ElementType::ACCIDENTAL,
        ElementType::SOUND_FLAG
    };

    for (EngravingItem* element : selectedElementList) {
        if ((!element->score()->isMaster() && !kfw::contains(noAvailableChangePartsSettingsTypes, element->type()))
            || element->canBeExcludedFromOtherParts()) {
            return true;
        }
    }

    return false;
}

bool AbstractInspectorModel::isEmpty() const
{
    return m_elementList.isEmpty();
}

void AbstractInspectorModel::setTitle(QString title)
{
    if (m_title == title) {
        return;
    }

    m_title = title;
    emit titleChanged();
}

void AbstractInspectorModel::setIcon(kfw::ui::IconCode::Code icon)
{
    m_icon = icon;
}

void AbstractInspectorModel::setSectionType(InspectorSectionType sectionType)
{
    m_sectionType = sectionType;
}

void AbstractInspectorModel::setModelType(InspectorModelType modelType)
{
    m_modelType = modelType;
}

void AbstractInspectorModel::onPropertyValueChanged(const kn::engraving::Pid pid, const QVariant& newValue)
{
    setPropertyValue(m_elementList, pid, newValue);
    loadProperties();
}

void AbstractInspectorModel::setPropertyValue(const QList<engraving::EngravingItem*>& items, const kn::engraving::Pid pid,
                                              const QVariant& newValue)
{
    if (items.empty()) {
        return;
    }

    beginCommand(TranslatableString("undoableAction", "Edit %1").arg(propertyUserName(pid)));

    for (kn::engraving::EngravingItem* item : items) {
        IF_ASSERT_FAILED(item) {
            continue;
        }

        kn::engraving::PropertyFlags ps = item->propertyFlags(pid);

        if (ps == kn::engraving::PropertyFlags::STYLED) {
            ps = kn::engraving::PropertyFlags::UNSTYLED;
        }

        PropertyValue propValue = valueToElementUnits(pid, newValue, item);
        item->undoChangeProperty(pid, propValue, ps);
    }

    updateNotation();
    endCommand();
}

void AbstractInspectorModel::onPropertyValueReset(const kn::engraving::Pid pid)
{
    resetPropertyValue(m_elementList, pid);
    loadProperties();
}

void AbstractInspectorModel::resetPropertyValue(const QList<engraving::EngravingItem*>& items, const kn::engraving::Pid pid)
{
    if (items.empty()) {
        return;
    }

    beginCommand(TranslatableString("undoableAction", "Reset %1").arg(propertyUserName(pid)));

    for (kn::engraving::EngravingItem* item : items) {
        IF_ASSERT_FAILED(item) {
            continue;
        }

        item->undoResetProperty(pid);
    }

    updateNotation();
    endCommand();
}

void AbstractInspectorModel::updateProperties()
{
    requestElements();

    emit isEmptyChanged();

    if (!isEmpty()) {
        loadProperties();
    }
}

void AbstractInspectorModel::requestElements()
{
    if (m_elementType != kn::engraving::ElementType::INVALID) {
        m_elementList = m_repository->findElementsByType(m_elementType);
    }
}

void AbstractInspectorModel::disconnectAll()
{
    async_disconnectAll();
}

void AbstractInspectorModel::onCurrentNotationChanged()
{
}

void AbstractInspectorModel::onNotationChanged(const kn::engraving::PropertyIdSet&, const kn::engraving::StyleIdSet&)
{
}

kn::engraving::Sid AbstractInspectorModel::styleIdByPropertyId(const kn::engraving::Pid pid) const
{
    kn::engraving::Sid result = kn::engraving::Sid::NOSTYLE;

    for (const kn::engraving::EngravingItem* element : m_elementList) {
        result = element->getPropertyStyle(pid);

        if (result != kn::engraving::Sid::NOSTYLE) {
            break;
        }
    }

    return result;
}

kn::engraving::PropertyIdSet AbstractInspectorModel::propertyIdSetFromStyleIdSet(const StyleIdSet& styleIdSet) const
{
    if (styleIdSet.empty()) {
        return PropertyIdSet();
    }

    PropertyIdSet result;

    for (const kn::engraving::EngravingItem* element : m_elementList) {
        const kn::engraving::ElementStyle* style = element->styledProperties();
        if (!style) {
            continue;
        }

        for (const StyledProperty& property : *style) {
            if (kfw::contains(styleIdSet, property.sid)) {
                result.insert(property.pid);
            }
        }
    }

    return result;
}

bool AbstractInspectorModel::updateStyleValue(const kn::engraving::Sid& sid, const QVariant& newValue)
{
    PropertyValue newVal = PropertyValue::fromQVariant(newValue, kn::engraving::MStyle::valueType(sid));
    if (style() && style()->styleValue(sid) != newVal) {
        beginCommand(TranslatableString("undoableAction", "Edit style"));
        style()->setStyleValue(sid, newVal);
        endCommand();
        return true;
    }

    return false;
}

QVariant AbstractInspectorModel::styleValue(const kn::engraving::Sid& sid) const
{
    return style() ? style()->styleValue(sid).toQVariant() : QVariant();
}

PropertyValue AbstractInspectorModel::valueToElementUnits(const kn::engraving::Pid& pid, const QVariant& value,
                                                          const kn::engraving::EngravingItem* element) const
{
    if (kn::engraving::Pid::VERSE == pid) {
        return value.toInt() - 1;
    }

    auto toPoint = [](const QVariant& v) {
        return PointF::fromQPointF(v.value<QPointF>());
    };

    P_TYPE type = kn::engraving::propertyType(pid);
    switch (type) {
    case P_TYPE::POINT: {
        if (pid == Pid::OFFSET ? element->offsetIsSpatiumDependent() : element->sizeIsSpatiumDependent()) {
            return toPoint(value) * element->spatium();
        } else {
            return toPoint(value) * kn::engraving::DPMM;
        }
    }

    case P_TYPE::MILLIMETRE:
        return Spatium(value.toReal()).toMM(element->spatium());

    case P_TYPE::SPATIUM:
        return Spatium(value.toReal());

    case P_TYPE::TEMPO:
        return BeatsPerSecond::fromBPM(BeatsPerMinute(value.toReal()));

    case P_TYPE::INT_VEC: {
        bool ok = true;
        std::vector<int> res;

        for (const QString& str : value.toString().split(',', Qt::SkipEmptyParts)) {
            if (int i = str.simplified().toInt(&ok); ok) {
                res.push_back(i);
            }
        }

        return res;
    } break;

    case P_TYPE::COLOR:
        return Color::fromQColor(value.value<QColor>());

    default:
        return PropertyValue::fromQVariant(value, type);
    }
}

QVariant AbstractInspectorModel::valueFromElementUnits(const kn::engraving::Pid& pid, const PropertyValue& value,
                                                       const kn::engraving::EngravingItem* element) const
{
    if (kn::engraving::Pid::VERSE == pid) {
        return value.toInt() + 1;
    }

    switch (value.type()) {
    case P_TYPE::POINT: {
        if (pid == Pid::OFFSET ? element->offsetIsSpatiumDependent() : element->sizeIsSpatiumDependent()) {
            return value.value<PointF>().toQPointF() / element->spatium();
        } else {
            return value.value<PointF>().toQPointF() / kn::engraving::DPMM;
        }
    }

    case P_TYPE::MILLIMETRE:
        return Spatium::fromMM(value.toReal(), element->spatium()).val();

    case P_TYPE::SPATIUM:
        return value.value<Spatium>().val();

    case P_TYPE::TEMPO:
        return value.value<BeatsPerSecond>().toBPM().val;

    case P_TYPE::DIRECTION_V:
        return static_cast<int>(value.value<kn::engraving::DirectionV>());

    case P_TYPE::INT_VEC: {
        QStringList strList;

        for (const int i : value.value<std::vector<int> >()) {
            strList << QString::number(i);
        }

        return strList.join(",");
    }
    case P_TYPE::COLOR:
        return value.value<kfw::draw::Color>().toQColor();
    default:
        return value.toQVariant();
    }
}

void AbstractInspectorModel::setElementType(kn::engraving::ElementType type)
{
    m_elementType = type;
}

PropertyItem* AbstractInspectorModel::buildPropertyItem(const kn::engraving::Pid& propertyId,
                                                        std::function<void(const kn::engraving::Pid propertyId,
                                                                           const QVariant& newValue)> onPropertyChangedCallBack,
                                                        std::function<void(const kn::engraving::Sid styleId,
                                                                           const QVariant& newValue)> onStyleChangedCallBack,
                                                        std::function<void(const kn::engraving::Pid propertyId)> onPropertyResetCallBack)
{
    PropertyItem* newPropertyItem = new PropertyItem(propertyId, this);

    initPropertyItem(newPropertyItem, onPropertyChangedCallBack, onStyleChangedCallBack, onPropertyResetCallBack);

    return newPropertyItem;
}

PointFPropertyItem* AbstractInspectorModel::buildPointFPropertyItem(const kn::engraving::Pid& propertyId,
                                                                    std::function<void(const kn::engraving::Pid propertyId,
                                                                                       const QVariant& newValue)> onPropertyChangedCallBack,
                                                                    std::function<void(const kn::engraving::Pid propertyId)> onPropertyResetCallBack)
{
    PointFPropertyItem* newPropertyItem = new PointFPropertyItem(propertyId, this);

    initPropertyItem(newPropertyItem, onPropertyChangedCallBack, nullptr, onPropertyResetCallBack);

    return newPropertyItem;
}

void AbstractInspectorModel::initPropertyItem(PropertyItem* propertyItem,
                                              std::function<void(const kn::engraving::Pid propertyId,
                                                                 const QVariant& newValue)> onPropertyChangedCallBack,
                                              std::function<void(const kn::engraving::Sid styleId,
                                                                 const QVariant& newValue)> onStyleChangedCallBack,
                                              std::function<void(const kn::engraving::Pid propertyId)> onPropertyResetCallBack)
{
    auto propertyCallback = onPropertyChangedCallBack;
    if (!propertyCallback) {
        propertyCallback = [this](const kn::engraving::Pid propertyId, const QVariant& newValue) {
            onPropertyValueChanged(propertyId, newValue);
        };
    }

    auto styleCallback = onStyleChangedCallBack;
    if (!styleCallback) {
        styleCallback = [this](const kn::engraving::Sid styleId, const QVariant& newValue) {
            updateStyleValue(styleId, newValue);

            emit requestReloadPropertyItems();
        };
    }

    auto resetCallback = onPropertyResetCallBack;
    if (!resetCallback) {
        resetCallback = [this](const kn::engraving::Pid propertyId) {
            onPropertyValueReset(propertyId);
        };
    }

    connect(propertyItem, &PropertyItem::propertyModified, this, propertyCallback);
    connect(propertyItem, &PropertyItem::resetToDefaultRequested, this, resetCallback);
    connect(propertyItem, &PropertyItem::applyToStyleRequested, this, styleCallback);
}

void AbstractInspectorModel::loadPropertyItem(PropertyItem* propertyItem, ConvertPropertyValueFunc convertElementPropertyValueFunc)
{
    loadPropertyItem(propertyItem, m_elementList, convertElementPropertyValueFunc);
}

void AbstractInspectorModel::loadPropertyItem(PropertyItem* propertyItem, const QList<EngravingItem*>& elements,
                                              ConvertPropertyValueFunc convertElementPropertyValueFunc)
{
    if (!propertyItem) {
        return;
    }

    if (elements.isEmpty()) {
        propertyItem->setIsEnabled(false);
        return;
    }

    kn::engraving::Pid pid = propertyItem->propertyId();

    kn::engraving::Sid styleId = styleIdByPropertyId(pid);
    propertyItem->setStyleId(styleId);

    QVariant propertyValue;

    bool isUndefined = false;
    bool isModified = false;

    for (const kn::engraving::EngravingItem* element : elements) {
        IF_ASSERT_FAILED(element) {
            continue;
        }

        QVariant elementCurrentValue = valueFromElementUnits(pid, element->getProperty(pid), element);
        QVariant elementDefaultValue = valueFromElementUnits(pid, element->propertyDefault(pid), element);

        bool isPropertySupportedByElement = elementCurrentValue.isValid();

        if (!isPropertySupportedByElement) {
            continue;
        }

        if (convertElementPropertyValueFunc) {
            elementCurrentValue = convertElementPropertyValueFunc(elementCurrentValue);
            elementDefaultValue = convertElementPropertyValueFunc(elementDefaultValue);
        }

        if (!propertyValue.isValid()) {
            propertyValue = elementCurrentValue;
        }

        if (!isUndefined && propertyValue != elementCurrentValue) {
            isUndefined = true;
        }

        if (!isModified) {
            PropertyFlags f = element->propertyFlags(pid);
            if (f == PropertyFlags::UNSTYLED || elementCurrentValue != elementDefaultValue) {
                isModified = true;
            }
        }

        if (isUndefined && isModified) {
            break;
        }
    }

    //@note Some elements may support the property, some don't. If element doesn't support property it'll return invalid value.
    //      So we use that knowledge here
    propertyItem->setIsEnabled(propertyValue.isValid());

    if (isUndefined) {
        propertyValue = QVariant();
    }

    propertyItem->updateCurrentValue(propertyValue);
    propertyItem->setIsModified(isModified);
}

bool AbstractInspectorModel::isNotationExisting() const
{
    return context()->currentProject() != nullptr;
}

INotationStylePtr AbstractInspectorModel::style() const
{
    return currentNotation() ? currentNotation()->style() : nullptr;
}

INotationUndoStackPtr AbstractInspectorModel::undoStack() const
{
    return currentNotation() ? currentNotation()->undoStack() : nullptr;
}

void AbstractInspectorModel::beginCommand(const kfw::TranslatableString& actionName)
{
    if (undoStack()) {
        undoStack()->prepareChanges(actionName);
    }

    //! NOTE prevents unnecessary updating of properties
    //! after changing their values in the inspector
    m_shouldUpdateOnScoreChange = false;
}

void AbstractInspectorModel::endCommand()
{
    if (undoStack()) {
        undoStack()->commitChanges();
    }

    m_shouldUpdateOnScoreChange = true;
}

INotationPtr AbstractInspectorModel::currentNotation() const
{
    return context()->currentNotation();
}

bool AbstractInspectorModel::isMasterNotation() const
{
    INotationPtr notation = currentNotation();
    if (!notation) {
        return false;
    }

    return notation == context()->currentMasterNotation()->notation();
}

INotationSelectionPtr AbstractInspectorModel::selection() const
{
    INotationPtr notation = currentNotation();
    return notation ? notation->interaction()->selection() : nullptr;
}

void AbstractInspectorModel::updateNotation()
{
    if (!currentNotation()) {
        return;
    }

    currentNotation()->notationChanged().notify();
}
