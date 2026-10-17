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

#include <qqmlintegration.h>

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"

#include "abstractinspectormodel.h"
#include "fretframechordlistmodel.h"

namespace kn::engraving {
class FBox;
}

namespace kn::inspector {
class FretFrameChordsSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable from QML")

    Q_PROPERTY(kn::inspector::FretFrameChordListModel * chordListModel READ chordListModel CONSTANT)

    Q_PROPERTY(kn::inspector::PropertyItem * listOrder READ listOrder CONSTANT)

    Q_PROPERTY(bool hasInvisibleChords READ hasInvisibleChords NOTIFY hasInvisibleChordsChanged)

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    explicit FretFrameChordsSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                          IElementRepositoryService* repository);

    FretFrameChordListModel* chordListModel() const;

    PropertyItem* listOrder() const;
    bool hasInvisibleChords() const;

    Q_INVOKABLE void resetList();

signals:
    void hasInvisibleChordsChanged(bool hasInvisible);

private:
    engraving::FBox* fretBox() const;

    void createProperties() override;
    void requestElements() override;
    void loadProperties() override;
    void resetProperties() override;
    void onNotationChanged(const kn::engraving::PropertyIdSet& changedPropertyIdSet,
                           const kn::engraving::StyleIdSet& changedStyleIdSet) override;

    void loadProperties(const kn::engraving::PropertyIdSet& propertyIdSet);

    void updateHasInvisibleChords();

    std::shared_ptr<FretFrameChordListModel> m_chordListModel;

    PropertyItem* m_listOrder = nullptr;
    bool m_hasInvisibleChords = false;
};
}
