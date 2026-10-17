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
#ifndef KN_CONTEXT_GLOBALCONTEXT_H
#define KN_CONTEXT_GLOBALCONTEXT_H

#include "../iglobalcontext.h"

#include "playbackstate.h"

namespace kn::context {
class GlobalContext : public IGlobalContext
{
public:

    GlobalContext();

    void setCurrentProject(const project::INotationProjectPtr& project) override;
    project::INotationProjectPtr currentProject() const override;
    kfw::async::Notification currentProjectChanged() const override;

    notation::IMasterNotationPtr currentMasterNotation() const override;
    kfw::async::Notification currentMasterNotationChanged() const override;

    void setCurrentNotation(const notation::INotationPtr& notation) override;
    notation::INotationPtr currentNotation() const override;
    kfw::async::Notification currentNotationChanged() const override;

    void setCurrentPlayer(const kfw::audio::IPlayerPtr& player) override;
    IPlaybackStatePtr playbackState() const override;

private:
    void doSetCurrentNotation(const notation::INotationPtr& notation);

    project::INotationProjectPtr m_currentProject;
    kfw::async::Notification m_currentProjectChanged;

    notation::INotationPtr m_currentNotation;
    kfw::async::Notification m_currentNotationChanged;

    std::shared_ptr<PlaybackState> m_playbackState;
};
}

#endif // KN_CONTEXT_GLOBALCONTEXT_H
