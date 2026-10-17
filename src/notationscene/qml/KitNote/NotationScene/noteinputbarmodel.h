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
#pragma once

#include <QQmlParserStatus>
#include <qqmlintegration.h>

#include "notation/inotation.h"
#include "uicomponents/qml/KitNote/UiComponents/abstractmenumodel.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "playback/iplaybackcontroller.h"
#include "ui/iuiconfiguration.h"

namespace kn::notation {
class NoteInputBarModel : public kfw::uicomponents::AbstractMenuModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus);
    QML_ELEMENT;

    Q_PROPERTY(bool isInputAllowed READ isInputAllowed NOTIFY isInputAllowedChanged)

    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::ContextInject<context::IGlobalContext> context = { this };
    kfw::ContextInject<playback::IPlaybackController> playbackController = { this };

public:
    explicit NoteInputBarModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool isInputAllowed() const;

signals:
    void isInputAllowedChanged();

private:
    enum NoteInputRoles {
        OrderRole = AbstractMenuModel::Roles::UserRole + 1,
        SectionRole
    };

    void classBegin() override;
    void componentComplete() override {}
    void init();

    void setNotation(const INotationPtr& notation);

    void load() override;

    void updateItemStateChecked(kfw::uicomponents::MenuItem& item, bool checked);
    void updateItemsStateChecked(const kfw::actions::ActionCode& actionCode, bool checked);

    void updateState();
    void updateNoteInputState();
    void updateNoteInputModeState();
    void updateNoteDotState();
    void updateNoteDurationState();
    void updateNoteAccidentalState();
    void updateTieState();
    void updateLvState();
    void updateSlurState();
    void updateVoicesState();
    void updateArticulationsState();
    void updateRestState();
    void updateAddState();

    kfw::uicomponents::MenuItem* makeActionItem(const kfw::ui::UiAction& action, const QString& section,
                                                 const kfw::uicomponents::MenuItemList& subitems = {});
    kfw::uicomponents::MenuItem* makeAddItem(const QString& section);

    kfw::uicomponents::MenuItemList makeCrossStaffBeamingItems();
    kfw::uicomponents::MenuItemList makeTupletItems();
    kfw::uicomponents::MenuItemList makeAddItems();
    kfw::uicomponents::MenuItemList makeNotesItems();
    kfw::uicomponents::MenuItemList makeIntervalsItems();
    kfw::uicomponents::MenuItemList makeMeasuresItems();
    kfw::uicomponents::MenuItemList makeFramesItems();
    kfw::uicomponents::MenuItemList makeFramesAppendItems();
    kfw::uicomponents::MenuItemList makeTextItems();
    kfw::uicomponents::MenuItemList makeLinesItems();
    kfw::uicomponents::MenuItemList makeChordAndFretboardDiagramsItems();

    INotationNoteInputPtr noteInput() const;
    INotationInteractionPtr interaction() const;
    INotationSelectionPtr selection() const;
    INotationUndoStackPtr undoStack() const;

    int resolveCurrentVoiceIndex() const;
    std::set<SymbolId> resolveCurrentArticulations() const;
    bool resolveRestSelected() const;
    DurationType resolveCurrentDurationType() const;

    bool isNoteInputMode() const;
    const NoteInputState& noteInputState() const;

    const ChordRest* elementToChordRest(const EngravingItem* element) const;

    INotationPtr m_notation = nullptr;
};
}
