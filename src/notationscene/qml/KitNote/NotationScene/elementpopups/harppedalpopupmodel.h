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

#include <QObject>

#include "context/iglobalcontext.h"

#include "engraving/dom/harppedaldiagram.h"

#include "../abstractelementpopupmodel.h"

namespace kn::notation {
class HarpPedalPopupModel : public AbstractElementPopupModel
{
    Q_OBJECT
    QML_ELEMENT;

    Q_PROPERTY(bool isDiagram READ isDiagram WRITE setIsDiagram NOTIFY isDiagramChanged)
    Q_PROPERTY(
        QVector<kn::notation::HarpPedalPopupModel::Position> pedalState READ pedalState WRITE setDiagramPedalState NOTIFY pedalStateChanged)
    Q_PROPERTY(QRectF staffPos READ staffPos CONSTANT)

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    enum class Position {
        FLAT,
        NATURAL,
        SHARP,

        UNSET
    };
    Q_ENUM(Position)

    explicit HarpPedalPopupModel(QObject* parent = nullptr);

    bool isDiagram() const;

    QRectF staffPos() const;

    QVector<Position> pedalState() const;

    Q_INVOKABLE void init() override;

public slots:
    void setIsDiagram(bool isDiagram);
    void setDiagramPedalState(QVector<kn::notation::HarpPedalPopupModel::Position> pedalState);

signals:
    void isDiagramChanged(bool isDiagram);
    void pedalStateChanged(QVector<kn::notation::HarpPedalPopupModel::Position> pedalState);

private:
    void load();

    // Convert between kn::engraving::PedalPosition and internal qml safe Position enums
    void setPopupPedalState(std::array<kn::engraving::PedalPosition, kn::engraving::HARP_STRING_NO> pos);

    void setPopupPedalState(std::array<HarpPedalPopupModel::Position, kn::engraving::HARP_STRING_NO> pos);

    std::array<kn::engraving::PedalPosition, kn::engraving::HARP_STRING_NO> getPopupPedalState();

    bool m_isDiagram = false;

    std::array<Position, kn::engraving::HARP_STRING_NO> m_pedalState;
};
} //namespace kn::notation
