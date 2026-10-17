/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include <memory>

#include "global/io/path.h"
#include "global/types/ret.h"
#include "global/async/channel.h"

#include "iprojectaudiosettings.h"
#include "notation/imasternotation.h"
#include "types/projecttypes.h"

namespace kn::project {
class INotationProject
{
public:
    virtual ~INotationProject() = default;

    virtual kfw::io::path_t path() const = 0;
    virtual void setPath(const kfw::io::path_t& path) = 0;
    virtual kfw::async::Notification pathChanged() const = 0;

    virtual QString displayName() const = 0;
    virtual kfw::async::Notification displayNameChanged() const = 0;

    virtual kfw::Ret load(const kfw::io::path_t& path, const OpenParams& params = {}, const std::string& format = "") = 0;
    virtual kfw::Ret createNew(const ProjectCreateOptions& projectInfo) = 0;

    virtual bool isNewlyCreated() const = 0;
    virtual void markAsNewlyCreated() = 0;

    virtual bool isImported() const = 0;

    virtual void markAsUnsaved() = 0;

    virtual kfw::ValNt<bool> needSave() const = 0;
    virtual kfw::Ret canSave() const = 0;

    virtual bool needAutoSave() const = 0;
    virtual void setNeedAutoSave(bool val) = 0;

    virtual kfw::Ret save(const kfw::io::path_t& path = kfw::io::path_t(), SaveMode saveMode = SaveMode::Save,
                           bool createBackup = true) = 0;
    virtual kfw::Ret savePage(const kfw::io::path_t& path, const size_t pageNum) = 0;
    virtual kfw::async::Channel<kfw::io::path_t, SaveMode> saveComplited() const = 0;

    virtual kfw::Ret writeToDevice(QIODevice* device) = 0;

    virtual ProjectMeta metaInfo() const = 0;
    virtual void setMetaInfo(const ProjectMeta& meta, bool undoable = false) = 0;

    virtual notation::IMasterNotationPtr masterNotation() const = 0;
    virtual IProjectAudioSettingsPtr audioSettings() const = 0;
};

using INotationProjectPtr = std::shared_ptr<INotationProject>;
}
