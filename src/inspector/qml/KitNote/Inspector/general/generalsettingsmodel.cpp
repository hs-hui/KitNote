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
#include "generalsettingsmodel.h"

#include "engraving/dom/fret.h"
#include "engraving/dom/harmony.h"

#include "translation.h"

using namespace kn::inspector;
using namespace kn::engraving;

GeneralSettingsModel::GeneralSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                           IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    createProperties();

    setTitle(kfw::qtrc("inspector", "General"));
    setSectionType(InspectorSectionType::SECTION_GENERAL);

    m_playbackProxyModel = new PlaybackProxyModel(this, iocCtx, repository);
    m_playbackProxyModel->init();

    m_appearanceSettingsModel = new AppearanceSettingsModel(this, iocCtx, repository);
    m_appearanceSettingsModel->init();
}

void GeneralSettingsModel::createProperties()
{
    m_isVisible = buildPropertyItem(Pid::VISIBLE, [this](const kn::engraving::Pid, const QVariant& newValue) {
        onVisibleChanged(newValue.toBool());
    });

    m_isSmall = buildPropertyItem(Pid::SMALL, [this](const kn::engraving::Pid, const QVariant& newValue) {
        setPropertyValue(m_elementsForIsSmallProperty, Pid::SMALL, newValue);
    });

    m_isAutoPlaceAllowed = buildPropertyItem(Pid::AUTOPLACE);
    m_isPlayable = buildPropertyItem(Pid::PLAY);
}

void GeneralSettingsModel::requestElements()
{
    m_elementList = m_repository->takeAllElements();

    QSet<EngravingItem*> elementsForIsSmallProperty;
    QSet<EngravingItem*> elementsForIsVisibleProperty;
    QSet<EngravingItem*> elementsForIsAutoPlaceProperty;
    QSet<EngravingItem*> elementsForIsPlayableProperty;

    for (EngravingItem* element : m_elementList) {
        if (element->isHarmony() && toHarmony(element)->isInFretBox()) {
            continue;
        }
        if (element->isFretDiagram() && toFretDiagram(element)->isInFretBox()) {
            continue;
        }

        // Trill cue note is small by definition, so isSmall property does not apply
        if (element->isNote() && toNote(element)->isTrillCueNote()) {
            continue;
        }

        EngravingItem* chord = element->findAncestor(ElementType::CHORD);

        if (chord) {
            elementsForIsSmallProperty.insert(chord);
        } else {
            elementsForIsSmallProperty.insert(element);
        }

        elementsForIsVisibleProperty.insert(element);
        elementsForIsAutoPlaceProperty.insert(element);
        elementsForIsPlayableProperty.insert(element);
    }

    m_elementsForIsSmallProperty = elementsForIsSmallProperty.values();
    m_elementsForIsVisibleProperty = elementsForIsVisibleProperty.values();
    m_elementsForIsAutoPlaceProperty = elementsForIsAutoPlaceProperty.values();
    m_elementsForIsPlayableProperty = elementsForIsPlayableProperty.values();
}

void GeneralSettingsModel::loadProperties()
{
    static const PropertyIdSet propertyIdSet {
        Pid::VISIBLE,
        Pid::AUTOPLACE,
        Pid::PLAY,
        Pid::SMALL,
    };

    loadProperties(propertyIdSet);
    updateAreGeneralPropertiesAvailable();
}

void GeneralSettingsModel::resetProperties()
{
    m_isVisible->resetToDefault();
    m_isAutoPlaceAllowed->resetToDefault();
    m_isPlayable->resetToDefault();
    m_isSmall->resetToDefault();
}

void GeneralSettingsModel::onNotationChanged(const PropertyIdSet& changedPropertyIdSet, const StyleIdSet&)
{
    loadProperties(changedPropertyIdSet);
}

void GeneralSettingsModel::loadProperties(const kn::engraving::PropertyIdSet& propertyIdSet)
{
    if (kfw::contains(propertyIdSet, Pid::VISIBLE)) {
        loadPropertyItem(m_isVisible, m_elementsForIsVisibleProperty);
    }

    if (kfw::contains(propertyIdSet, Pid::AUTOPLACE)) {
        loadPropertyItem(m_isAutoPlaceAllowed, m_elementsForIsAutoPlaceProperty);
    }

    if (kfw::contains(propertyIdSet, Pid::PLAY)) {
        bool isMaster = isMasterNotation();
        m_isPlayable->setIsVisible(isMaster);

        if (isMaster) {
            loadPropertyItem(m_isPlayable, m_elementsForIsPlayableProperty);
        }
    }

    if (kfw::contains(propertyIdSet, Pid::SMALL)) {
        loadPropertyItem(m_isSmall, m_elementsForIsSmallProperty);
    }
}

void GeneralSettingsModel::onCurrentNotationChanged()
{
    m_appearanceSettingsModel->onCurrentNotationChanged();
    m_playbackProxyModel->onCurrentNotationChanged();
}

void GeneralSettingsModel::onVisibleChanged(bool visible)
{
    const kfw::TranslatableString actionName = visible
                                                ? TranslatableString("undoableAction", "Make element(s) visible")
                                                : TranslatableString("undoableAction", "Make element(s) invisible");

    beginCommand(actionName);

    Score* score = currentNotation()->elements()->msScore();

    for (EngravingItem* item : m_elementList) {
        score->undoChangeVisible(item, visible);
    }

    updateNotation();
    endCommand();
}

PropertyItem* GeneralSettingsModel::isVisible() const
{
    return m_isVisible;
}

PropertyItem* GeneralSettingsModel::isAutoPlaceAllowed() const
{
    return m_isAutoPlaceAllowed;
}

PropertyItem* GeneralSettingsModel::isPlayable() const
{
    return m_isPlayable;
}

PropertyItem* GeneralSettingsModel::isSmall() const
{
    return m_isSmall;
}

PlaybackProxyModel* GeneralSettingsModel::playbackProxyModel() const
{
    return m_playbackProxyModel;
}

AppearanceSettingsModel* GeneralSettingsModel::appearanceSettingsModel() const
{
    return m_appearanceSettingsModel;
}

bool GeneralSettingsModel::areGeneralPropertiesAvailable()
{
    return m_areGeneralPropertiesAvailable;
}

void GeneralSettingsModel::updateAreGeneralPropertiesAvailable()
{
    static const std::set<ElementType> TYPES_NO_PROPERTIES = {
        ElementType::LAYOUT_BREAK,
        ElementType::SYSTEM_LOCK_INDICATOR
    };

    bool available = true;
    for (const EngravingItem* item : m_elementList) {
        if (item && kfw::contains(TYPES_NO_PROPERTIES, item->type())) {
            available = false;
            break;
        }
    }

    if (m_areGeneralPropertiesAvailable != available) {
        m_areGeneralPropertiesAvailable = available;
        emit areGeneralPropertiesAvailableChanged(m_areGeneralPropertiesAvailable);
    }
}
