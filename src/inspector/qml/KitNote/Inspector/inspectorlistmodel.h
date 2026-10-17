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

#include <memory>

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <qqmlintegration.h>

#include "engraving/dom/engravingitem.h"

#include "modularity/ioc.h"
#include "async/asyncable.h"
#include "context/iglobalcontext.h"
#include "abstractinspectormodel.h"

namespace kn::inspector {
class IElementRepositoryService;
class InspectorListModel : public QAbstractListModel, public QQmlParserStatus, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    QML_ELEMENT

    kfw::ContextInject<context::IGlobalContext> context = { this };

public:
    explicit InspectorListModel(QObject* parent = nullptr);
    ~InspectorListModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    Q_INVOKABLE void setInspectorVisible(bool visible);

private:
    enum RoleNames {
        InspectorSectionModelRole = Qt::UserRole + 1
    };

    void classBegin() override;
    void componentComplete() override {}
    void init();

    void listenSelectionChanged();
    void listenScoreChanges();

    void onScoreChanged(const kn::engraving::PropertyIdSet& changedPropertyIdSet, const kn::engraving::StyleIdSet& changedStyleIdSet);

    void updateElementList();

    bool alwaysUpdateModelList(const QList<kn::engraving::EngravingItem*>& selectedElementList);
    void setElementList(const QList<kn::engraving::EngravingItem*>& selectedElementList,
                        notation::SelectionState selectionState = notation::SelectionState::NONE);

    void buildModelsForEmptySelection();
    void buildModelsForSelectedElements(const ElementKeySet& selectedElementKeySet, bool isRangeSelection,
                                        const QList<engraving::EngravingItem*>& selectedElementList);

    void createModelsBySectionType(const InspectorSectionTypeSet& sectionTypes, const ElementKeySet& selectedElementKeySet = {});
    void removeUnusedModels(const ElementKeySet& newElementKeySet, bool isRangeSelection,
                            const QList<kn::engraving::EngravingItem*>& selectedElementList,
                            const InspectorSectionTypeSet& exclusions = {});

    bool isModelAllowed(const AbstractInspectorModel* model, const InspectorModelTypeSet& allowedModelTypes,
                        const InspectorSectionTypeSet& allowedSectionTypes) const;

    void sortModels();

    AbstractInspectorModel* modelBySectionType(InspectorSectionType sectionType) const;

    void notifyModelsAboutNotationChanged();

    QList<AbstractInspectorModel*> m_modelList;

    std::unique_ptr<IElementRepositoryService> m_repository;

    bool m_inspectorVisible = true;
    kn::engraving::PropertyIdSet m_changedPropertyIdSet;
    kn::engraving::StyleIdSet m_changedStyleIdSet;
};
}
