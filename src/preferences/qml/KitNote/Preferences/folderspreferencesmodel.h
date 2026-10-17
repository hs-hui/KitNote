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

#include <qqmlintegration.h>

#include <QAbstractListModel>

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "iglobalconfiguration.h"
#include "project/iprojectconfiguration.h"
#include "notation/inotationconfiguration.h"
#include "audio/main/iaudioconfiguration.h"
#include "vst/ivstconfiguration.h"
#include "audioplugins/iregisteraudiopluginsscenario.h"

namespace kn::preferences {
class FoldersPreferencesModel : public QAbstractListModel, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool vstEnabled READ vstEnabled CONSTANT)

    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::GlobalInject<project::IProjectConfiguration> projectConfiguration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<kfw::audio::IAudioConfiguration> audioConfiguration;
    kfw::GlobalInject<kfw::vst::IVstConfiguration> vstConfiguration;
    kfw::ContextInject<kfw::audioplugins::IRegisterAudioPluginsScenario> registerAudioPluginsScenario = { this };

public:
    explicit FoldersPreferencesModel(QObject* parent = nullptr);

    bool vstEnabled() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void load();
    Q_INVOKABLE void rescanVstPlugins();

private:
    void setupConnections();

    enum Roles {
        TitleRole = Qt::UserRole + 1,
        PathRole,
        DirRole,
        IsMultiDirectoriesRole
    };

    enum class FolderType {
        Undefined,
        Scores,
        Styles,
        Instruments,
        Templates,
        SoundFonts,
        MusicFonts,
        VST3
    };

    enum class FolderValueType {
        Directory,
        MultiDirectories
    };

    struct FolderInfo {
        FolderType type = FolderType::Undefined;
        QString title;
        QString value;
        QString dir;
        FolderValueType valueType = FolderValueType::Directory;
    };

    void saveFolderPaths(FolderType folderType, const QString& paths);

    void setFolderPaths(FolderType folderType, const QString& paths);
    QModelIndex folderIndex(FolderType folderType);

    QString pathsToString(const kfw::io::paths_t& paths) const;
    kfw::io::paths_t pathsFromString(const QString& pathsStr) const;

    QList<FolderInfo> m_folders;
};
}
