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

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"

#include "notation/notationtypes.h"

namespace kn::instrumentsscene {
class InstrumentSettingsModel : public QObject, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(QString instrumentName READ instrumentName WRITE setInstrumentName NOTIFY dataChanged)
    Q_PROPERTY(QString abbreviature READ abbreviature WRITE setAbbreviature NOTIFY dataChanged)
    Q_PROPERTY(int hideWhenEmpty READ hideWhenEmpty WRITE setHideWhenEmpty NOTIFY hideWhenEmptyChanged)
    Q_PROPERTY(
        bool hideStavesWhenIndividuallyEmpty READ hideStavesWhenIndividuallyEmpty WRITE setHideStavesWhenIndividuallyEmpty NOTIFY hideStavesWhenIndividuallyEmptyChanged)

    Q_PROPERTY(bool hasMultipleStaves READ hasMultipleStaves NOTIFY hasMultipleStavesChanged)
    Q_PROPERTY(bool isMainScore READ isMainScore NOTIFY isMainScoreChanged)

    QML_ELEMENT

    kfw::ContextInject<context::IGlobalContext> context = { this };

public:
    explicit InstrumentSettingsModel(QObject* parent = nullptr);

    Q_INVOKABLE void load(const QVariant& instrument);

    QString instrumentName() const;
    QString abbreviature() const;
    int hideWhenEmpty() const;
    bool hideStavesWhenIndividuallyEmpty() const;
    bool hasMultipleStaves() const;

    bool isMainScore() const;

public slots:
    void setInstrumentName(const QString& name);
    void setAbbreviature(const QString& abbreviature);
    void setHideWhenEmpty(int value);
    void setHideStavesWhenIndividuallyEmpty(bool value);

signals:
    void dataChanged();
    void hideWhenEmptyChanged();
    void hideStavesWhenIndividuallyEmptyChanged();
    void hasMultipleStavesChanged();
    void isMainScoreChanged();

private:
    notation::INotationPtr currentNotation() const;
    notation::INotationPtr currentMasterNotation() const;
    notation::INotationPartsPtr notationParts() const;

    notation::InstrumentKey m_instrumentKey;
    QString m_instrumentName;
    QString m_instrumentAbbreviature;
    int m_hideWhenEmpty = 0; // AutoOnOff::AUTO
    bool m_hideStavesWhenIndividuallyEmpty = false;
    bool m_hasMultipleStaves = false;
};
}
