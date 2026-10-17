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
//! KitNote: the pattern library - the grooves, fills and rudiments of "Add > Drum patterns" and
//! "Add > Rudiments", each with a picture of what it writes (a measure of it as a small step grid).
//! A pattern is written by its command, so the card, the menu and the command search do the same.
class PatternLibraryModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT

    //! "My patterns": measures the user has kept. They are stored as one-measure drum tabs in the
    //! settings, so they are there for every score.
    Q_PROPERTY(QVariantList userPatterns READ userPatterns NOTIFY userPatternsChanged)

    //! The score has no drum note yet (for the word of welcome on an empty score). It is looked at
    //! only while "watchEmptyScore" is set: once the user has begun, nobody asks any more.
    Q_PROPERTY(bool scoreEmpty READ scoreEmpty NOTIFY scoreEmptyChanged)
    Q_PROPERTY(bool watchEmptyScore READ watchEmptyScore WRITE setWatchEmptyScore NOTIFY watchEmptyScoreChanged)

    QML_ELEMENT

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::ui::IUiActionsRegister> actionsRegister = { this };

public:
    explicit PatternLibraryModel(QObject* parent = nullptr);

    //! group: 0 grooves, 1 fills, 2 rudiments.
    //! A pattern: { code, title, steps, group, sticking, rows: [ { label, family, states } ] }
    Q_INVOKABLE QVariantList patterns(int group) const;

    QVariantList userPatterns() const;

    Q_INVOKABLE void load();
    bool scoreEmpty() const { return m_scoreEmpty; }
    bool watchEmptyScore() const { return m_watchEmptyScore; }
    void setWatchEmptyScore(bool watch);

    //! Keeps the selected measure under a name; false when there is no measure with drum notes to keep.
    Q_INVOKABLE bool saveSelectedMeasure(const QString& name);
    //! The number of measures written: 0 when the selection has no measure of the length of the pattern.
    Q_INVOKABLE int applyUserPattern(int index);
    Q_INVOKABLE void removeUserPattern(int index);

    //! Writes a fill made up on the spot into the selected measures: sixteenth notes from the snare down
    //! the toms; with "halfMeasure" after two beats of the rock beat. Another one at every call.
    Q_INVOKABLE int writeRandomFill(bool halfMeasure);
    static QString randomFillTab(bool halfMeasure, quint32 seed);

signals:
    void userPatternsChanged();
    void scoreEmptyChanged();
    void watchEmptyScoreChanged();
    void scoreOpened();

private:
    void updateScoreEmpty();

    bool m_scoreEmpty = false;
    bool m_watchEmptyScore = false;
};
}
