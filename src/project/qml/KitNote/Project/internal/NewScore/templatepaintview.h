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

#include <qqmlintegration.h>

#include "modularity/ioc.h"
#include "notationscene/qml/KitNote/NotationScene/abstractnotationpaintview.h"
#include "project/iprojectcreator.h"
#include "shortcuts/ishortcutsregister.h"

namespace kn::project {
class TemplatePaintView : public notation::AbstractNotationPaintView
{
    Q_OBJECT

    QML_ELEMENT

    kfw::GlobalInject<IProjectCreator> notationCreator;
    kfw::ContextInject<kfw::shortcuts::IShortcutsRegister> shortcutsRegister = { this };

public:
    explicit TemplatePaintView(QQuickItem* parent = nullptr);
    ~TemplatePaintView() override;

    Q_INVOKABLE void load(const QString& templatePath);

    Q_INVOKABLE QString zoomInSequence() const;
    Q_INVOKABLE QString zoomOutSequence() const;

private slots:
    void onViewSizeChanged() override;

private:
    void onNotationSetup() override;

    void resetNotation();

    QString shortcutsTitleByActionCode(const kfw::actions::ActionCode& code) const;

    void adjustCanvas();
    qreal resolveDefaultScaling() const;

    QString m_templatePath;

    INotationProjectPtr m_notationProject = nullptr;
};
}
