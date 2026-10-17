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

#include <QHash>
#include <qqmlintegration.h>

#include "abstractinspectormodel.h"

namespace kn::inspector {
class AbstractInspectorProxyModel : public AbstractInspectorModel
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable from QML")

    Q_PROPERTY(bool isMultiModel READ isMultiModel NOTIFY modelsChanged)
    Q_PROPERTY(QVariantList models READ models NOTIFY modelsChanged)
    Q_PROPERTY(kn::inspector::AbstractInspectorModel * firstModel READ firstModel NOTIFY modelsChanged)

    Q_PROPERTY(
        kn::inspector::AbstractInspectorModel::InspectorModelType defaultSubModelType READ defaultSubModelType NOTIFY defaultSubModelTypeChanged)

public:
    explicit AbstractInspectorProxyModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                         IElementRepositoryService* repository);

    bool isMultiModel() const;
    QVariantList models() const;
    AbstractInspectorModel* firstModel() const;

    InspectorModelType defaultSubModelType() const;

    Q_INVOKABLE kn::inspector::AbstractInspectorModel* modelByType(InspectorModelType type) const;

    QList<AbstractInspectorModel*> modelList() const;

    void createProperties() override {}
    void loadProperties() override {}
    void resetProperties() override {}

    void requestElements() override;
    void requestResetToDefaults() override;
    void disconnectAll() override;
    bool isEmpty() const override;

    void updateModels(const ElementKeySet& newElementKeySet);
    bool shouldUpdateWhenEmpty() const override;
    bool shouldUpdateOnEmptyPropertyAndStyleIdSets() const override;

    void onCurrentNotationChanged() override;

    void onNotationChanged(const kn::engraving::PropertyIdSet& changedPropertyIdSet,
                           const kn::engraving::StyleIdSet& changedStyleIdSet) override;

public slots:
    void setDefaultSubModelType(kn::inspector::InspectorModelType modelType);

signals:
    void modelsChanged();
    void defaultSubModelTypeChanged();

protected:
    void setModels(const QList<AbstractInspectorModel*>& models);

private:
    QHash<InspectorModelType, AbstractInspectorModel*> m_models;
    InspectorModelType m_defaultSubModelType = InspectorModelType::TYPE_UNDEFINED;
};
}
