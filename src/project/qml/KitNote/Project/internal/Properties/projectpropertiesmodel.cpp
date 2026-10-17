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
#include "projectpropertiesmodel.h"

#include "translation.h"
#include "log.h"

using namespace kfw;
using namespace kfw::modularity;
using namespace kn::notation;
using namespace kn::project;

ProjectPropertiesModel::ProjectPropertiesModel(QObject* parent)
    : QAbstractListModel(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void ProjectPropertiesModel::classBegin()
{
    init();
}

void ProjectPropertiesModel::init()
{
    INotationProjectPtr project = context()->currentProject();
    if (project) {
        m_projectMetaInfo = project->metaInfo();
    }
}

void ProjectPropertiesModel::load()
{
    beginResetModel();

    QVariantMap additionalProperties = m_projectMetaInfo.additionalTags;

    m_properties = {
        { WORK_TITLE_TAG, kfw::qtrc("project", "Work title"), m_projectMetaInfo.title, true },
        { SUBTITLE_TAG, kfw::qtrc("project", "Subtitle"), m_projectMetaInfo.subtitle, true },
        { COMPOSER_TAG, kfw::qtrc("project", "Composer"), m_projectMetaInfo.composer, true },
        { ARRANGER_TAG, kfw::qtrc("project", "Arranger"), m_projectMetaInfo.arranger, true },
        { LYRICIST_TAG, kfw::qtrc("project", "Lyricist"), m_projectMetaInfo.lyricist, true },
        { TRANSLATOR_TAG, kfw::qtrc("project", "Translator"), m_projectMetaInfo.translator, true },
        { COPYRIGHT_TAG, kfw::qtrc("project", "Copyright"), m_projectMetaInfo.copyright, /*isStandard*/ true, /*isMultiLineEdit*/ true },
        { WORK_NUMBER_TAG, kfw::qtrc("project", "Work number"), additionalProperties[WORK_NUMBER_TAG].toString(), true },
        { MOVEMENT_TITLE_TAG, kfw::qtrc("project", "Movement title"), additionalProperties[MOVEMENT_TITLE_TAG].toString(), true },
        { MOVEMENT_NUMBER_TAG, kfw::qtrc("project", "Movement number"), additionalProperties[MOVEMENT_NUMBER_TAG].toString(), true },
        { CREATION_DATE_TAG, kfw::qtrc("project", "Creation date"), m_projectMetaInfo.creationDate.toString(), true },
        { PLATFORM_TAG, kfw::qtrc("project", "Platform"), m_projectMetaInfo.platform, true }
    };

    for (const QString& propertyName : additionalProperties.keys()) {
        if (!isStandardTag(propertyName)) {
            m_properties.append({ "", propertyName, additionalProperties[propertyName].toString() });
        }
    }

    endResetModel();
}

QVariant ProjectPropertiesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount()) {
        return QVariant();
    }

    const Property& property = m_properties[index.row()];

    switch (role) {
    case PropertyName:
        return property.name;
    case PropertyValue:
        return property.value;
    case IsStandardProperty:
        return property.isStandardProperty;
    case IsMultiLineEdit:
        return property.isMultiLineEdit;
    }

    return QVariant();
}

bool ProjectPropertiesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() >= rowCount()) {
        return false;
    }

    Property& property = m_properties[index.row()];

    switch (role) {
    case PropertyName:
        if (!value.canConvert<QString>()) {
            return false;
        }
        property.name = value.toString();
        return true;
    case PropertyValue:
        if (!value.canConvert<QString>()) {
            return false;
        }
        property.value = value.toString();
        return true;
    default:
        return false;
    }
}

int ProjectPropertiesModel::rowCount(const QModelIndex&) const
{
    return m_properties.size();
}

QHash<int, QByteArray> ProjectPropertiesModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { PropertyName, "propertyName" },
        { PropertyValue, "propertyValue" },
        { IsStandardProperty, "isStandardProperty" },
        { IsMultiLineEdit, "isMultiLineEdit" }
    };

    return roles;
}

QString ProjectPropertiesModel::filePath() const
{
    return m_projectMetaInfo.filePath.toQString();
}

QString ProjectPropertiesModel::version() const
{
    return m_projectMetaInfo.kitnoteVersion;
}

QString ProjectPropertiesModel::revision() const
{
    int rev = m_projectMetaInfo.kitnoteRevision;
    QString revision;
    if (rev > 99999) {     // MuseScore 1.3 is decimal 5702, 2.0 and later uses a 7-digit hex SHA
        revision = QString::number(rev, 16);
    } else {
        revision = QString::number(rev, 10);
    }

    return revision;
}

QString ProjectPropertiesModel::apiLevel() const
{
    return QString::number(m_projectMetaInfo.mscVersion);
}

void ProjectPropertiesModel::newProperty()
{
    int destinationIndex = m_properties.size();
    beginInsertRows(QModelIndex(), destinationIndex, destinationIndex);

    Property property = { "", "", "" };
    m_properties.append(property);

    endInsertRows();

    emit propertyAdded(destinationIndex);
}

void ProjectPropertiesModel::deleteProperty(int index)
{
    if (index < 0 && index >= m_properties.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);

    m_properties.removeAt(index);

    endRemoveRows();
}

void ProjectPropertiesModel::saveProperties()
{
    INotationProjectPtr project = context()->currentProject();
    if (!project) {
        return;
    }

    ProjectMeta meta = project->metaInfo();
    meta.additionalTags = {};

    for (const Property& property : m_properties) {
        if (property.key == WORK_TITLE_TAG) {
            meta.title = property.value;
        } else if (property.key == SUBTITLE_TAG) {
            meta.subtitle = property.value;
        } else if (property.key == COMPOSER_TAG) {
            meta.composer = property.value;
        } else if (property.key == ARRANGER_TAG) {
            meta.arranger = property.value;
        } else if (property.key == LYRICIST_TAG) {
            meta.lyricist = property.value;
        } else if (property.key == TRANSLATOR_TAG) {
            meta.translator = property.value;
        } else if (property.key == COPYRIGHT_TAG) {
            meta.copyright = property.value;
        } else if (property.key == CREATION_DATE_TAG) {
            meta.creationDate = QDate::fromString(property.value);
        } else if (property.key == SOURCE_TAG) {
            meta.source = property.value;
        } else if (property.key == PLATFORM_TAG) {
            meta.platform = property.value;
        } else if (!property.key.isEmpty()) {
            assert(!isRepresentedInProjectMeta(property.key));
            meta.additionalTags[property.key] = property.value;
        } else if (!property.name.isEmpty() && !property.value.isEmpty()) {
            meta.additionalTags[property.name] = property.value;
        }
    }

    project->setMetaInfo(meta, true);
}

void ProjectPropertiesModel::openFileLocation()
{
    Ret ret = interactive()->revealInFileBrowser(m_projectMetaInfo.filePath.toQString());

    if (!ret) {
        LOGE() << "Could not open folder: " << m_projectMetaInfo.filePath.toQString();
    }
}
