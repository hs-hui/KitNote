/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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

#include "actions/actionable.h"
#include "uicomponents/qml/KitNote/UiComponents/abstractmenumodel.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"
#include "inotationsceneconfiguration.h"

namespace kfw {
class TranslatableString;
}

namespace kn::notation {
class PianoKeyboardPanelContextMenuModel : public kfw::uicomponents::AbstractMenuModel, public kfw::actions::Actionable
{
    Q_OBJECT

    Q_PROPERTY(int numberOfKeys READ numberOfKeys NOTIFY numberOfKeysChanged)
    Q_PROPERTY(qreal keyWidthScaling READ keyWidthScaling WRITE setKeyWidthScaling NOTIFY keyWidthScalingChanged)

    QML_ELEMENT

    kfw::GlobalInject<INotationSceneConfiguration> configuration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };

public:
    explicit PianoKeyboardPanelContextMenuModel(QObject* parent = nullptr);

    Q_INVOKABLE void load() override;

    qreal keyWidthScaling() const;
    void setKeyWidthScaling(qreal scaling);

    int numberOfKeys() const;

signals:
    void keyWidthScalingChanged();
    void setKeyWidthScalingRequested(qreal scaling);
    void numberOfKeysChanged();

private:
    kfw::uicomponents::MenuItem* makeViewMenu();

    kfw::uicomponents::MenuItem* makeKeyWidthScalingItem(const kfw::TranslatableString& title, qreal scaling);
    kfw::uicomponents::MenuItem* makeNumberOfKeysItem(const kfw::TranslatableString& title, int numberOfKeys);

    void updateKeyWidthScalingItems();

    kfw::uicomponents::MenuItemList m_keyWidthScalingItems;

    qreal m_currentKeyWidthScaling = 0.0;
};
}
