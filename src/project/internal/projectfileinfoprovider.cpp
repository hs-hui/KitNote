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

#include "projectfileinfoprovider.h"

#include "notationproject.h"

using namespace kfw;
using namespace kn::project;

ProjectFileInfoProvider::ProjectFileInfoProvider(NotationProject* project)
    : m_project(project)
{
}

bool ProjectFileInfoProvider::saved() const
{
    return !m_project->needSave().val;
}

//! TODO: maybe implement this class further
kfw::io::path_t ProjectFileInfoProvider::path() const
{
    return m_project->path();
}

kfw::io::path_t ProjectFileInfoProvider::fileName(bool includingExtension) const
{
    return io::filename(path(), includingExtension);
}

kfw::io::path_t ProjectFileInfoProvider::absoluteDirPath() const
{
    return io::absoluteDirpath(path());
}

String ProjectFileInfoProvider::displayName() const
{
    return m_project->displayName();
}

DateTime ProjectFileInfoProvider::birthTime() const
{
    return filesystem()->birthTime(path());
}

DateTime ProjectFileInfoProvider::lastModified() const
{
    return filesystem()->lastModified(path());
}

bool ProjectFileInfoProvider::isNewlyCreated() const
{
    return m_project->isNewlyCreated();
}
