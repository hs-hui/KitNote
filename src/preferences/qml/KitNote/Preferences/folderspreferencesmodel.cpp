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
#include "folderspreferencesmodel.h"

#include "translation.h"

#include "kfw_framework_config.h"

using namespace kn::preferences;
using namespace kfw;

FoldersPreferencesModel::FoldersPreferencesModel(QObject* parent)
    : QAbstractListModel(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

bool FoldersPreferencesModel::vstEnabled() const
{
#ifdef KFW_MODULE_VST
    return true;
#else
    return false;
#endif
}

int FoldersPreferencesModel::rowCount(const QModelIndex&) const
{
    return m_folders.count();
}

QVariant FoldersPreferencesModel::data(const QModelIndex& index, int role) const
{
    const FolderInfo& folder = m_folders.at(index.row());
    switch (role) {
    case TitleRole: return folder.title;
    case PathRole: return folder.value;
    case DirRole: return folder.dir;
    case IsMultiDirectoriesRole: return folder.valueType == FolderValueType::MultiDirectories;
    }

    return QVariant();
}

bool FoldersPreferencesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    const FolderInfo& folder = m_folders.at(index.row());

    switch (role) {
    case PathRole:
        if (folder.value == value.toString()) {
            return false;
        }

        saveFolderPaths(folder.type, value.toString());
        return true;
    default:
        break;
    }

    return false;
}

QHash<int, QByteArray> FoldersPreferencesModel::roleNames() const
{
    static const QHash<int, QByteArray> roles = {
        { TitleRole, "title" },
        { PathRole, "path" },
        { DirRole, "dir" },
        { IsMultiDirectoriesRole, "isMultiDirectories" }
    };

    return roles;
}

void FoldersPreferencesModel::load()
{
    beginResetModel();

    m_folders = {
        {
            FolderType::Scores, kfw::qtrc("preferences", "Scores"), projectConfiguration()->userProjectsPath().toQString(),
            projectConfiguration()->userProjectsPath().toQString()
        },
        {
            FolderType::Styles, kfw::qtrc("preferences", "Styles"), notationConfiguration()->userStylesPath().toQString(),
            notationConfiguration()->userStylesPath().toQString()
        },
        {
            FolderType::Instruments, kfw::qtrc("preferences", "Instruments and score orders"),
            notationConfiguration()->userInstrumentsFolder().toQString(),
            notationConfiguration()->userInstrumentsFolder().toQString()
        },
        {
            FolderType::Templates, kfw::qtrc("preferences", "Templates"), projectConfiguration()->userTemplatesPath().toQString(),
            projectConfiguration()->userTemplatesPath().toQString()
        },
        {
            FolderType::SoundFonts, kfw::qtrc("preferences", "SoundFonts"), pathsToString(
                audioConfiguration()->userSoundFontDirectories()),
            globalConfiguration()->userDataPath().toQString(), FolderValueType::MultiDirectories
        },
        {
            FolderType::MusicFonts, kfw::qtrc("preferences", "Musical symbol fonts"),
            notationConfiguration()->userMusicFontsPath().toQString(),
            notationConfiguration()->userMusicFontsPath().toQString()
        },
#ifdef KFW_MODULE_VST
        {
            FolderType::VST3, kfw::qtrc("preferences", "VST3"), pathsToString(vstConfiguration()->userVstDirectories()),
            globalConfiguration()->userDataPath().toQString(), FolderValueType::MultiDirectories
        }
#endif
    };

    endResetModel();

    setupConnections();
}

void FoldersPreferencesModel::setupConnections()
{
    projectConfiguration()->userProjectsPathChanged().onReceive(this, [this](const kfw::io::path_t& path) {
        setFolderPaths(FolderType::Scores, path.toQString());
    });

    notationConfiguration()->userStylesPathChanged().onReceive(this, [this](const kfw::io::path_t& path) {
        setFolderPaths(FolderType::Styles, path.toQString());
    });

    notationConfiguration()->userInstrumentsFolderChanged().onReceive(this, [this](const kfw::io::path_t& path) {
        setFolderPaths(FolderType::Instruments, path.toQString());
    });

    projectConfiguration()->userTemplatesPathChanged().onReceive(this, [this](const kfw::io::path_t& path) {
        setFolderPaths(FolderType::Templates, path.toQString());
    });

    audioConfiguration()->soundFontDirectoriesChanged().onReceive(this, [this](const io::paths_t&) {
        io::paths_t userSoundFontsPaths = audioConfiguration()->userSoundFontDirectories();
        setFolderPaths(FolderType::SoundFonts, pathsToString(userSoundFontsPaths));
    });

    notationConfiguration()->userMusicFontsPathChanged().onReceive(this, [this](const kfw::io::path_t& path) {
        setFolderPaths(FolderType::MusicFonts, path.toQString());
    });

    vstConfiguration()->userVstDirectoriesChanged().onReceive(this, [this](const io::paths_t& paths) {
        setFolderPaths(FolderType::VST3, pathsToString(paths));
    });
}

void FoldersPreferencesModel::saveFolderPaths(FoldersPreferencesModel::FolderType folderType, const QString& paths)
{
    switch (folderType) {
    case FolderType::Scores: {
        kfw::io::path_t folderPath = paths.toStdString();
        projectConfiguration()->setUserProjectsPath(folderPath);
        break;
    }
    case FolderType::Styles: {
        kfw::io::path_t folderPath = paths.toStdString();
        notationConfiguration()->setUserStylesPath(folderPath);
        break;
    }
    case FolderType::Instruments: {
        kfw::io::path_t folderPath = paths.toStdString();
        notationConfiguration()->setUserInstrumentsFolder(folderPath);
        break;
    }
    case FolderType::Templates: {
        kfw::io::path_t folderPath = paths.toStdString();
        projectConfiguration()->setUserTemplatesPath(folderPath);
        break;
    }
    case FolderType::SoundFonts: {
        audioConfiguration()->setUserSoundFontDirectories(pathsFromString(paths));
        break;
    }
    case FolderType::MusicFonts: {
        notationConfiguration()->setUserMusicFontsPath(paths.toStdString());
        break;
    }
    case FolderType::VST3: {
        vstConfiguration()->setUserVstDirectories(pathsFromString(paths));
        break;
    }
    case FolderType::Undefined:
        break;
    }
}

void FoldersPreferencesModel::setFolderPaths(FoldersPreferencesModel::FolderType folderType, const QString& paths)
{
    QModelIndex index = folderIndex(folderType);
    if (!index.isValid()) {
        return;
    }

    m_folders[index.row()].value = paths;
    emit dataChanged(index, index, { PathRole });
}

QModelIndex FoldersPreferencesModel::folderIndex(FoldersPreferencesModel::FolderType folderType)
{
    for (int i = 0; i < m_folders.count(); ++i) {
        FolderInfo& folderInfo = m_folders[i];
        if (folderInfo.type == folderType) {
            return index(i, 0);
        }
    }

    return QModelIndex();
}

void FoldersPreferencesModel::rescanVstPlugins()
{
    if (registerAudioPluginsScenario()) {
        registerAudioPluginsScenario()->rescanAllPlugins();
    }
}

QString FoldersPreferencesModel::pathsToString(const io::paths_t& paths) const
{
    return QString::fromStdString(io::pathsToString(paths));
}

io::paths_t FoldersPreferencesModel::pathsFromString(const QString& pathsStr) const
{
    return io::pathsFromString(pathsStr.toStdString());
}
