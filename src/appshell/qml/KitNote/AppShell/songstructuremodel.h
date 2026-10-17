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

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "ui/iuiactionsregister.h"

namespace kn::appshell {
//! KitNote: the structure of the chart - its sections (Intro, Verse, Chorus ...), each with the measure
//! it starts at and its length; a click goes there, and a section can be started at the selected measure.
//! (The reading and writing is INotationInteraction::chartSections and its neighbours.)
class SongStructureModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT

    //! The card is on screen. While it is not, the score is not read at every edit and selection.
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QVariantList sections READ sections NOTIFY sectionsChanged)

    QML_ELEMENT

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::ui::IUiActionsRegister> actionsRegister = { this };

public:
    explicit SongStructureModel(QObject* parent = nullptr);

    bool active() const { return m_active; }
    void setActive(bool active);

    QVariantList sections() const { return m_sections; }

    Q_INVOKABLE void load();
    Q_INVOKABLE void goTo(int index);
    Q_INVOKABLE void addSection(const QString& name);

    //! The grooves a section can be filled with: { title, pattern }
    Q_INVOKABLE QVariantList grooves() const;
    Q_INVOKABLE void fillCurrentSection(int pattern, bool fillAtEnd);

    //! Tap tempo: the tempo of the music in beats a minute (quarters; dotted quarters in 6/8)
    Q_INVOKABLE void setMusicTempo(int quartersPerMinute);

signals:
    void activeChanged();
    void sectionsChanged();

private:
    void onNotationChanged();
    void update();

    bool m_active = false;
    QVariantList m_sections;
};
}
