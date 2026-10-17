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
#ifndef KFW_WORKSPACE_WORKSPACESTUB_H
#define KFW_WORKSPACE_WORKSPACESTUB_H

#include "workspace/iworkspace.h"

namespace kfw::workspace {
class WorkspaceStub : public IWorkspace
{
public:
    std::string name() const override;

    bool isBuiltin() const override;
    bool isEdited() const override;
    bool isNeedSave() const override;

    RetVal<QByteArray> rawData(const DataKey& key) const override;
    Ret setRawData(const DataKey& key, const QByteArray& data) override;

    void reset() override;
    void assignNewName(const std::string& newName) override;

    async::Notification reloadNotification() override;
};
}

#endif // KFW_WORKSPACE_WORKSPACESTUB_H
