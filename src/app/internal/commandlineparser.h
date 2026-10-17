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
#ifndef KN_APP_COMMANDLINEPARSER_H
#define KN_APP_COMMANDLINEPARSER_H

#include <QCommandLineParser>
#include <QStringList>

#include "cmdoptions.h"

class QCoreApplication;

namespace kn::app {
class CommandLineParser
{
    //! NOTE: This parser is created at the earliest stage of the application initialization
    //! You should not inject anything into it
public:
    CommandLineParser() = default;

    void init();
    void parse(int argc, char** argv);
    void processBuiltinArgs(const QCoreApplication& app);

    kfw::IApplication::RunMode runMode() const;
    const CmdOptions& options() const;

private:
    void printLongVersion() const;

    QCommandLineParser m_parser;
    CmdOptions m_options;
};
}

#endif // KN_APP_COMMANDLINEPARSER_H
