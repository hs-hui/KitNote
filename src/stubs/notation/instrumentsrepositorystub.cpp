/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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
#include "instrumentsrepositorystub.h"

using namespace kn::notation;

const InstrumentTemplateList& InstrumentsRepositoryStub::instrumentTemplates() const
{
    static InstrumentTemplateList l;
    return l;
}

const InstrumentTemplate& InstrumentsRepositoryStub::instrumentTemplate(const kfw::String&) const
{
    static InstrumentTemplate t;
    return t;
}

const ScoreOrderList& InstrumentsRepositoryStub::orders() const
{
    static ScoreOrderList l;
    return l;
}

const ScoreOrder& InstrumentsRepositoryStub::order(const kfw::String&) const
{
    static ScoreOrder o;
    return o;
}

const InstrumentGenreList& InstrumentsRepositoryStub::genres() const
{
    static InstrumentGenreList l;
    return l;
}

const InstrumentGroupList& InstrumentsRepositoryStub::groups() const
{
    static InstrumentGroupList l;
    return l;
}

const InstrumentStringTuningsMap& InstrumentsRepositoryStub::stringTuningsPresets() const
{
    static InstrumentStringTuningsMap m;
    return m;
}
