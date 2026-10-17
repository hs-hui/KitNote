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
#include "importpreferencesmodel.h"

#include <QTextCodec>

#include "engraving/types/constants.h"

#include "translation.h"

using namespace kn::preferences;

ImportPreferencesModel::ImportPreferencesModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void ImportPreferencesModel::load()
{
    notationConfiguration()->styleFileImportPathChanged().onReceive(this, [this](const std::string& val) {
        emit styleFileImportPathChanged(QString::fromStdString(val));
    });

    oveConfiguration()->importOvertureCharsetChanged().onReceive(this, [this](const std::string& val) {
        emit currentOvertureCharsetChanged(QString::fromStdString(val));
    });

    musicXmlConfiguration()->importLayoutChanged().onReceive(this, [this](bool val) {
        emit importLayoutChanged(val);
    });

    musicXmlConfiguration()->importBreaksChanged().onReceive(this, [this](bool val) {
        emit importBreaksChanged(val);
    });

    musicXmlConfiguration()->needUseDefaultFontChanged().onReceive(this, [this](bool val) {
        emit needUseDefaultFontChanged(val);
    });

    musicXmlConfiguration()->inferTextTypeChanged().onReceive(this, [this](bool val) {
        emit inferTextTypeChanged(val);
    });

    midiImportExportConfiguration()->midiShortestNoteChanged().onReceive(this, [this](int val) {
        emit currentShortestNoteChanged(val);
    });

    midiImportExportConfiguration()->roundTempoChanged().onReceive(this, [this](bool val) {
        emit roundTempoChanged(val);
    });

    meiConfiguration()->meiImportLayoutChanged().onReceive(this, [this](bool val) {
        emit meiImportLayoutChanged(val);
    });

    musicXmlConfiguration()->needAskAboutApplyingNewStyleChanged().onReceive(this, [this](bool val) {
        emit needAskAboutApplyingNewStyleChanged(val);
    });
}

QVariantList ImportPreferencesModel::charsets() const
{
    QList<QByteArray> charsets = QTextCodec::availableCodecs();
    std::sort(charsets.begin(), charsets.end());

    QVariantList result;
    for (QByteArray charset: charsets) {
        result << QString(charset);
    }

    return result;
}

QVariantList ImportPreferencesModel::shortestNotes() const
{
    constexpr int division =  engraving::Constants::DIVISION;

    QVariantList result = {
        QVariantMap { { "title", kfw::qtrc("preferences", "Quarter") }, { "value", division } },
        QVariantMap { { "title", kfw::qtrc("preferences", "Eighth") }, { "value", division / 2 } },
        QVariantMap { { "title", kfw::qtrc("preferences", "16th") }, { "value", division / 4 } },
        QVariantMap { { "title", kfw::qtrc("preferences", "32nd") }, { "value", division / 8 } },
        QVariantMap { { "title", kfw::qtrc("preferences", "64th") }, { "value", division / 16 } },
        QVariantMap { { "title", kfw::qtrc("preferences", "128th") }, { "value", division / 32 } },
        QVariantMap { { "title", kfw::qtrc("preferences", "256th") }, { "value", division / 64 } },
        QVariantMap { { "title", kfw::qtrc("preferences", "512th") }, { "value", division / 128 } },
        QVariantMap { { "title", kfw::qtrc("preferences", "1024th") }, { "value", division / 256 } }
    };

    return result;
}

QStringList ImportPreferencesModel::stylePathFilter() const
{
    return { kfw::qtrc("preferences", "MuseScore style file") + " (*.mss)" };
}

QString ImportPreferencesModel::styleChooseTitle() const
{
    return kfw::qtrc("preferences", "Choose default style for imports");
}

QString ImportPreferencesModel::fileDirectory(const QString& filePath) const
{
    return kfw::io::dirpath(filePath.toStdString()).toQString();
}

QString ImportPreferencesModel::styleFileImportPath() const
{
    return notationConfiguration()->styleFileImportPath().toQString();
}

QString ImportPreferencesModel::currentOvertureCharset() const
{
    return QString::fromStdString(oveConfiguration()->importOvertureCharset());
}

bool ImportPreferencesModel::importLayout() const
{
    return musicXmlConfiguration()->importLayout();
}

bool ImportPreferencesModel::importBreaks() const
{
    return musicXmlConfiguration()->importBreaks();
}

bool ImportPreferencesModel::needUseDefaultFont() const
{
    return musicXmlConfiguration()->needUseDefaultFont();
}

bool ImportPreferencesModel::inferTextType() const
{
    return musicXmlConfiguration()->inferTextType();
}

int ImportPreferencesModel::currentShortestNote() const
{
    return midiImportExportConfiguration()->midiShortestNote();
}

bool ImportPreferencesModel::roundTempo() const
{
    return midiImportExportConfiguration()->roundTempo();
}

bool ImportPreferencesModel::needAskAboutApplyingNewStyle() const
{
    return musicXmlConfiguration()->needAskAboutApplyingNewStyle();
}

bool ImportPreferencesModel::meiImportLayout() const
{
    return meiConfiguration()->meiImportLayout();
}

bool ImportPreferencesModel::mnxRequireExactSchemaValidation() const
{
    return mnxConfiguration()->mnxRequireExactSchemaValidation();
}

void ImportPreferencesModel::setStyleFileImportPath(QString path)
{
    if (path == styleFileImportPath()) {
        return;
    }

    notationConfiguration()->setStyleFileImportPath(path.toStdString());
}

void ImportPreferencesModel::setCurrentOvertureCharset(QString charset)
{
    if (charset == currentOvertureCharset()) {
        return;
    }

    oveConfiguration()->setImportOvertureCharset(charset.toStdString());
}

void ImportPreferencesModel::setImportLayout(bool import)
{
    if (import == importLayout()) {
        return;
    }

    musicXmlConfiguration()->setImportLayout(import);
}

void ImportPreferencesModel::setImportBreaks(bool import)
{
    if (import == importBreaks()) {
        return;
    }

    musicXmlConfiguration()->setImportBreaks(import);
}

void ImportPreferencesModel::setNeedUseDefaultFont(bool value)
{
    if (value == needUseDefaultFont()) {
        return;
    }

    musicXmlConfiguration()->setNeedUseDefaultFont(value);
}

void ImportPreferencesModel::setInferTextType(bool value)
{
    if (value == inferTextType()) {
        return;
    }

    musicXmlConfiguration()->setInferTextType(value);
}

void ImportPreferencesModel::setCurrentShortestNote(int note)
{
    if (note == currentShortestNote()) {
        return;
    }

    midiImportExportConfiguration()->setMidiShortestNote(note);
}

void ImportPreferencesModel::setRoundTempo(bool value)
{
    if (value == roundTempo()) {
        return;
    }

    midiImportExportConfiguration()->setRoundTempo(value);
}

void ImportPreferencesModel::setNeedAskAboutApplyingNewStyle(bool value)
{
    if (value == needAskAboutApplyingNewStyle()) {
        return;
    }

    musicXmlConfiguration()->setNeedAskAboutApplyingNewStyle(value);
}

void ImportPreferencesModel::setMeiImportLayout(bool import)
{
    if (import == meiImportLayout()) {
        return;
    }

    meiConfiguration()->setMeiImportLayout(import);
}

void ImportPreferencesModel::setMnxRequireExactSchemaValidation(bool value)
{
    if (value == mnxRequireExactSchemaValidation()) {
        return;
    }

    mnxConfiguration()->setMnxRequireExactSchemaValidation(value);
    emit mnxRequireExactSchemaValidationChanged(value);
}
