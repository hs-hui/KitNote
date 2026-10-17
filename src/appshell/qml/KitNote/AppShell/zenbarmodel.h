/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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
#include <QVariantList>
#include <qqmlintegration.h>

#include "async/asyncable.h"
#include "actions/actionable.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"
#include "ui/iuiactionsregister.h"
#include "shortcuts/ishortcutsregister.h"
#include "global/iinteractive.h"

namespace kn::appshell {
//! KitNote: the model of the "zen" interface - the small bar that floats over the score and the
//! command search that reaches every command by name (see NotationPage/ZenBar.qml).
class ZenBarModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable, public kfw::actions::Actionable
{
    Q_OBJECT

    Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)

    QML_ELEMENT

    kfw::ContextInject<kfw::ui::IUiActionsRegister> actionsRegister = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<kfw::shortcuts::IShortcutsRegister> shortcutsRegister = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:
    explicit ZenBarModel(QObject* parent = nullptr);

    QVariantList items() const;

    Q_INVOKABLE void load();
    Q_INVOKABLE void dispatch(const QString& actionCode);

    //! Runs a command found with the search, and remembers it: the search offers the last ones first.
    Q_INVOKABLE void run(const QString& actionCode);

    //! The commands whose name or description contains the text; with no text, the drum commands.
    Q_INVOKABLE QVariantList search(const QString& text) const;


signals:
    void itemsChanged();
    void commandSearchRequested();
    void stepGridRequested();
    void practiceRequested();
    void songStructureRequested();
    void patternsRequested();

private:
    QVariantMap commandItem(const kfw::ui::UiAction& action) const;
    void followNoteInput();

    bool m_openedNoteInputBar = false;
};
}
