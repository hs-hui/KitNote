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
#include <QQmlParserStatus>
#include <qqmlintegration.h>

#include "actions/actionable.h"
#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "internal/paletteprovider.h"
#include "actions/iactionsdispatcher.h"

namespace kn::palette {
// TODO: Refactor entire palette module so that these are not necessary anymore
struct PaletteProviderForeign {
    Q_GADGET
    QML_FOREIGN(PaletteProvider)
    QML_NAMED_ELEMENT(PaletteProvider)
    QML_UNCREATABLE("Must be created in C++ only")
};

struct AbstractPaletteControllerForeign {
    Q_GADGET
    QML_FOREIGN(AbstractPaletteController)
    QML_NAMED_ELEMENT(PaletteController)
    QML_UNCREATABLE("Must be created in C++ only")
};

struct UserPaletteControllerForeign {
    Q_GADGET
    QML_FOREIGN(UserPaletteController)
    QML_NAMED_ELEMENT(UserPaletteController)
    QML_UNCREATABLE("Must be created in C++ only")
};

struct PaletteElementEditorForeign {
    Q_GADGET
    QML_FOREIGN(PaletteElementEditor)
    QML_NAMED_ELEMENT(PaletteElementEditor)
    QML_UNCREATABLE("Must be created in C++ only")
};

struct PaletteTreeModelForeign {
    Q_GADGET
    QML_FOREIGN(PaletteTreeModel)
    QML_NAMED_ELEMENT(PaletteTreeModel)
    QML_UNCREATABLE("Must be created in C++ only")
};

struct FilterPaletteTreeModelForeign {
    Q_GADGET
    QML_FOREIGN(FilterPaletteTreeModel)
    QML_NAMED_ELEMENT(FilterPaletteTreeModel)
    QML_UNCREATABLE("Must be created in C++ only")
};

class PaletteRootModel : public QObject, public QQmlParserStatus, public kfw::actions::Actionable, public kfw::async::Asyncable,
    public kfw::Contextable
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(kn::palette::PaletteProvider * paletteProvider READ paletteProvider_property CONSTANT)

    kfw::ContextInject<IPaletteProvider> paletteProvider = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };

    QML_ELEMENT

public:
    explicit PaletteRootModel(QObject* parent = nullptr);
    ~PaletteRootModel() override;

    PaletteProvider* paletteProvider_property() const;

signals:
    void paletteSearchRequested();
    void applyCurrentPaletteElementRequested();

private:
    void classBegin() override;
    void componentComplete() override {}
    void init();
};
}
