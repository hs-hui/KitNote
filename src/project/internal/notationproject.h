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

#include "../inotationproject.h"

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "io/ifilesystem.h"
#include "../iprojectconfiguration.h"
#include "inotationreadersregister.h"
#include "inotationwritersregister.h"

#include "engraving/engravingproject.h"
#include "engraving/rendering/iscorerenderer.h"

#include "notation/inotationconfiguration.h"
#include "projectaudiosettings.h"
#include "iprojectmigrator.h"

#include "global/iglobalconfiguration.h"
#include "context/iglobalcontext.h"

namespace kn::engraving {
class MscReader;
class MscWriter;

namespace write {
class WriteContext;
}
}

namespace kn::project {
class NotationProject : public INotationProject, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;
    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<INotationReadersRegister> readers = { this };
    kfw::ContextInject<INotationWritersRegister> writers = { this };
    kfw::ContextInject<IProjectMigrator> migrator = { this };
    kfw::ContextInject<engraving::rendering::IScoreRenderer> renderer = { this };

public:
    NotationProject(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}
    ~NotationProject() override;

    static QString scoreDefaultTitle();

    kfw::Ret load(const kfw::io::path_t& path, const OpenParams& params = {}, const std::string& format = "") override;
    kfw::Ret createNew(const ProjectCreateOptions& projectInfo) override;

    kfw::io::path_t path() const override;
    void setPath(const kfw::io::path_t& path) override;
    kfw::async::Notification pathChanged() const override;

    QString displayName() const override;
    kfw::async::Notification displayNameChanged() const override;



    bool isNewlyCreated() const override;
    void markAsNewlyCreated() override;

    bool isImported() const override;

    void markAsUnsaved() override;

    kfw::ValNt<bool> needSave() const override;
    kfw::Ret canSave() const override;

    bool needAutoSave() const override;
    void setNeedAutoSave(bool val) override;

    kfw::Ret save(
        const kfw::io::path_t& path = kfw::io::path_t(), SaveMode saveMode = SaveMode::Save, bool createBackup = true) override;
    kfw::Ret savePage(const kfw::io::path_t& path, const size_t pageNum) override;
    kfw::async::Channel<kfw::io::path_t, SaveMode> saveComplited() const override;

    kfw::Ret writeToDevice(QIODevice* device) override;

    ProjectMeta metaInfo() const override;
    void setMetaInfo(const ProjectMeta& meta, bool undoable = false) override;

    notation::IMasterNotationPtr masterNotation() const override;
    IProjectAudioSettingsPtr audioSettings() const override;

private:
    void setupProject();

    kfw::Ret loadTemplate(const ProjectCreateOptions& projectOptions);

    kfw::Ret doLoad(const kfw::io::path_t& path, const OpenParams& params, const std::string& format);
    kfw::Ret doImport(const kfw::io::path_t& path, const OpenParams& params);

    kfw::String keepRecordingFoundFrom(const kfw::io::path_t& savePath); // KitNote
    kfw::Ret saveScore(const kfw::io::path_t& path, const std::string& fileSuffix, bool generateBackup = true,
                        bool createThumbnail = true, bool isAutosave = false, const engraving::write::WriteContext* ctx = nullptr);
    kfw::Ret saveSelectionOnScore(const kfw::io::path_t& path = kfw::io::path_t());
    kfw::Ret exportProject(const kfw::io::path_t& path, const std::string& suffix);
    kfw::Ret doSave(const kfw::io::path_t& path, engraving::MscIoMode ioMode, bool generateBackup = true, bool createThumbnail = true,
                     bool isAutosave = false, const engraving::write::WriteContext* ctx = nullptr);
    kfw::Ret makeBackup(kfw::io::path_t filePath);
    kfw::Ret writeProject(const kfw::io::path_t& path, const engraving::write::WriteContext* ctx = nullptr);
    kfw::Ret writeProject(engraving::MscWriter& msczWriter, bool createThumbnail = true,
                           const engraving::write::WriteContext* ctx = nullptr);
    kfw::Ret checkSavedFileForCorruption(engraving::MscIoMode ioMode, const kfw::io::path_t& path, const kfw::io::path_t& scoreFileName);

    void listenIfNeedSaveChanges();
    void markAsSaved(const kfw::io::path_t& path);
    void setNeedSave(bool needSave);

    kn::engraving::EngravingProjectPtr m_engravingProject = nullptr;
    notation::IMasterNotationPtr m_masterNotation = nullptr;
    ProjectAudioSettingsPtr m_projectAudioSettings = nullptr;

    kfw::io::path_t m_path;
    kfw::async::Notification m_pathChanged;
    kfw::async::Notification m_displayNameChanged;

    kfw::async::Notification m_needSaveNotification;

    kfw::async::Channel<kfw::io::path_t, SaveMode> m_saved;

    bool m_isNewlyCreated = false; /// true if the file has never been saved yet
    bool m_isImported = false;
    bool m_needSave = false;
    bool m_needAutoSave = false;
    bool m_hasNonUndoStackChanges = false;
};
}
