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

#include "async/asyncable.h"
#include "uicomponents/qml/KitNote/UiComponents/widgetview.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "actions/iactionsdispatcher.h"
#include "notation/inotationconfiguration.h"
#include "engraving/iengravingconfiguration.h"

namespace kn::palette {
class DrumsetPaletteAdapter;
class DrumsetPanelView : public kfw::uicomponents::WidgetView, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(QString pitchName READ pitchName NOTIFY pitchNameChanged)

    QML_ELEMENT

    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<engraving::IEngravingConfiguration> engravingConfiguration;
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };

public:
    explicit DrumsetPanelView(QQuickItem* parent = nullptr);

    QString pitchName() const;

    Q_INVOKABLE void customizeKit();

signals:
    void pitchNameChanged();

private:
    void componentComplete() override;

    void initDrumsetPalette();
    void updateColors();

    void setPitchName(const QString& name);

    QString m_pitchName;

    std::shared_ptr<DrumsetPaletteAdapter> m_adapter;
};
}
