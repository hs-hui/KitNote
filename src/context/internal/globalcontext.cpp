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
#include "globalcontext.h"

#include "notation/imasternotation.h"

using namespace kn::context;
using namespace kn::project;
using namespace kn::notation;
using namespace kfw::async;

GlobalContext::GlobalContext()
{
    m_playbackState = std::make_shared<PlaybackState>();
}

void GlobalContext::setCurrentProject(const INotationProjectPtr& project)
{
    if (m_currentProject == project) {
        return;
    }

    // KitNote: keep the old project alive until everyone has been told. Models that still hold its
    // notation (the status bar) let go of it in their handlers; when this was the last reference (the
    // app quitting with a score open, ContextModule::onDeinit) its score would already be deleted.
    const INotationProjectPtr oldProject = m_currentProject;
    m_currentProject = project;

    INotationPtr notation = project ? project->masterNotation()->notation() : nullptr;
    doSetCurrentNotation(notation);

    m_currentProjectChanged.notify();
    m_currentNotationChanged.notify();
}

INotationProjectPtr GlobalContext::currentProject() const
{
    return m_currentProject;
}

Notification GlobalContext::currentProjectChanged() const
{
    return m_currentProjectChanged;
}

IMasterNotationPtr GlobalContext::currentMasterNotation() const
{
    return m_currentProject ? m_currentProject->masterNotation() : nullptr;
}

Notification GlobalContext::currentMasterNotationChanged() const
{
    //! NOTE Same as project
    return m_currentProjectChanged;
}

void GlobalContext::setCurrentNotation(const INotationPtr& notation)
{
    if (m_currentNotation == notation) {
        return;
    }

    doSetCurrentNotation(notation);
    m_currentNotationChanged.notify();
}

INotationPtr GlobalContext::currentNotation() const
{
    return m_currentNotation;
}

Notification GlobalContext::currentNotationChanged() const
{
    return m_currentNotationChanged;
}

void GlobalContext::doSetCurrentNotation(const INotationPtr& notation)
{
    if (m_currentNotation == notation) {
        return;
    }

    m_currentNotation = notation;

    if (m_currentNotation) {
        m_currentNotation->setIsOpen(true);
    }
}

void GlobalContext::setCurrentPlayer(const kfw::audio::IPlayerPtr& player)
{
    m_playbackState->setPlayer(player);
}

IPlaybackStatePtr GlobalContext::playbackState() const
{
    return m_playbackState;
}
