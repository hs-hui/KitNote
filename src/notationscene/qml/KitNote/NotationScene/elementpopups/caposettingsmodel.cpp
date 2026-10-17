/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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

#include "caposettingsmodel.h"

#include "engraving/dom/capo.h"

using namespace kn::notation;

CapoSettingsModel::CapoSettingsModel(QObject* parent)
    : AbstractElementPopupModel(PopupModelType::TYPE_CAPO, parent)
{
}

uint8_t CapoSettingsModel::transposeMode() const
{
    return m_transposeMode;
}

bool CapoSettingsModel::capoIsOn() const
{
    return params().active;
}

int CapoSettingsModel::fretPosition() const
{
    return params().fretPosition;
}

QList<StringItem*> CapoSettingsModel::strings() const
{
    return m_strings;
}

int CapoSettingsModel::capoPlacement() const
{
    return m_item ? static_cast<int>(m_item->placement()) : 0;
}

bool CapoSettingsModel::capoTextSpecifiedByUser() const
{
    return m_item && !m_item->getProperty(kn::engraving::Pid::CAPO_GENERATE_TEXT).toBool();
}

QString CapoSettingsModel::userCapoText() const
{
    if (!capoTextSpecifiedByUser()) {
        return QString();
    }

    return m_item ? m_item->getProperty(kn::engraving::Pid::TEXT).value<kfw::String>().toQString() : QString();
}

void CapoSettingsModel::init()
{
    TRACEFUNC;

    m_strings.clear();

    AbstractElementPopupModel::init();

    IF_ASSERT_FAILED(m_item) {
        return;
    }

    const kn::engraving::Part* part = m_item->part();
    IF_ASSERT_FAILED(part) {
        return;
    }

    const kn::engraving::StringData* stringData = part->stringData(m_item->tick(), m_item->staff()->idx());
    IF_ASSERT_FAILED(stringData) {
        return;
    }

    for (kn::engraving::string_idx_t i = 0; i < stringData->strings(); ++i) {
        StringItem* item = new StringItem(this);
        item->blockSignals(true);
        item->setApplyCapo(!kfw::contains(params().ignoredStrings, i));
        item->blockSignals(false);

        m_strings.push_back(item);
    }

    m_transposeMode = static_cast<int>(params().transposeMode);

    emit capoIsOnChanged(capoIsOn());
    emit fretPositionChanged(fretPosition());
    emit stringsChanged(m_strings);
    emit capoPlacementChanged(capoPlacement());
    emit capoTextSpecifiedByUserChanged(capoTextSpecifiedByUser());
    emit userCapoTextChanged(userCapoText());
    emit transposeModeChanged(m_transposeMode);
}

void CapoSettingsModel::toggleCapoForString(int stringIndex)
{
    if (stringIndex >= m_strings.size()) {
        return;
    }

    StringItem* item = m_strings.at(stringIndex);
    item->setApplyCapo(!item->applyCapo());

    std::vector<int> ignoredStrings;
    for (int i = 0; i < m_strings.size(); ++i) {
        const StringItem* item2 = m_strings.at(i);

        if (!item2->applyCapo()) {
            ignoredStrings.push_back(i);
        }
    }

    changeItemProperty(kn::engraving::Pid::CAPO_IGNORED_STRINGS, ignoredStrings);
}

QVariantList CapoSettingsModel::possibleCapoPlacements() const
{
    QVariantMap above {
        { "text", kfw::qtrc("notation", "Above") },
        { "value", static_cast<int>(kn::engraving::PlacementV::ABOVE) }
    };

    QVariantMap below {
        { "text", kfw::qtrc("notation", "Below") },
        { "value", static_cast<int>(kn::engraving::PlacementV::BELOW) }
    };

    return {
        above,
        below
    };
}

void CapoSettingsModel::setTransposeMode(int mode)
{
    if (transposeMode() == mode) {
        return;
    }
    changeItemProperty(kn::engraving::Pid::CAPO_TRANSPOSE_MODE, mode);
    m_transposeMode = mode;

    emit transposeModeChanged(mode);
}

void CapoSettingsModel::setCapoIsOn(bool isOn)
{
    if (capoIsOn() == isOn) {
        return;
    }

    changeItemProperty(kn::engraving::Pid::ACTIVE, isOn);
    emit capoIsOnChanged(isOn);
}

void CapoSettingsModel::setFretPosition(int position)
{
    if (fretPosition() == position) {
        return;
    }

    changeItemProperty(kn::engraving::Pid::CAPO_FRET_POSITION, position);
    emit fretPositionChanged(position);
}

void CapoSettingsModel::setCapoPlacement(int placement)
{
    IF_ASSERT_FAILED(m_item) {
        return;
    }

    if (m_item->placement() == static_cast<kn::engraving::PlacementV>(placement)) {
        return;
    }

    changeItemProperty(kn::engraving::Pid::PLACEMENT, placement);
    emit capoPlacementChanged(placement);
}

void CapoSettingsModel::setCapoTextSpecifiedByUser(bool value)
{
    if (capoTextSpecifiedByUser() == value) {
        return;
    }

    changeItemProperty(kn::engraving::Pid::CAPO_GENERATE_TEXT, !value);
    emit capoTextSpecifiedByUserChanged(value);
}

void CapoSettingsModel::setUserCapoText(const QString& text)
{
    if (userCapoText() == text) {
        return;
    }

    changeItemProperty(kn::engraving::Pid::TEXT, kfw::String::fromQString(text));
    emit userCapoTextChanged(text);
}

const kn::engraving::CapoParams& CapoSettingsModel::params() const
{
    if (!m_item || !m_item->isCapo()) {
        static kn::engraving::CapoParams dummy;
        return dummy;
    }

    return kn::engraving::toCapo(m_item)->params();
}

StringItem::StringItem(QObject* parent)
    : QObject(parent)
{
}

bool StringItem::applyCapo() const
{
    return m_applyCapo;
}

void StringItem::setApplyCapo(bool apply)
{
    if (m_applyCapo == apply) {
        return;
    }

    m_applyCapo = apply;
    emit applyCapoChanged(apply);
}
