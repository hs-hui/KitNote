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

#include <QObject>
#include <qqmlintegration.h>

#include "modularity/ioc.h"

#include "async/asyncable.h"
#include "actions/actiontypes.h"
#include "audio/common/audiotypes.h"

namespace kn::playback {
class AbstractAudioResourceItem : public QObject, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(bool isBlank READ isBlank NOTIFY isBlankChanged)
    Q_PROPERTY(bool isActive READ isActive NOTIFY isActiveChanged)
    Q_PROPERTY(bool hasNativeEditorSupport READ hasNativeEditorSupport NOTIFY hasNativeEditorSupportChanged)

    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable as it is an abstract base class")

public:
    explicit AbstractAudioResourceItem(QObject* parent = nullptr);
    ~AbstractAudioResourceItem() override;

    Q_INVOKABLE void requestToLaunchNativeEditorView();
    void requestToCloseNativeEditorView();

    virtual Q_INVOKABLE void requestAvailableResources() {}
    virtual Q_INVOKABLE void handleMenuItem(const QString& menuItemId) { Q_UNUSED(menuItemId) }

    virtual QString title() const;
    virtual bool isBlank() const;
    virtual bool isActive() const;
    virtual bool hasNativeEditorSupport() const;

    const kfw::actions::ActionQuery& editorAction() const;
    void setEditorAction(const kfw::actions::ActionQuery& action);

signals:
    void titleChanged();
    void isBlankChanged();
    void isActiveChanged();
    void hasNativeEditorSupportChanged();

    void nativeEditorViewLaunchRequested();
    void nativeEditorViewCloseRequested();
    void availableResourceListResolved(const QVariantList& resources);

protected:
    QVariantMap buildMenuItem(const QString& itemId, const QString& title, const bool checked,
                              const QVariantList& subItems = QVariantList(), const bool includeInFilteredLists = true,
                              const bool isFilterCategory = false) const;

    QVariantMap buildSeparator() const;

    QVariantMap buildExternalLinkMenuItem(const QString& menuId, const QString& title) const;

    void sortResourcesList(kfw::audio::AudioResourceMetaList& list);

private:
    void doRequestToLaunchNativeEditorView();

    kfw::actions::ActionQuery m_editorAction;
};
}
