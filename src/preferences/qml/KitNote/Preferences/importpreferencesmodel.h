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

#include <QObject>

#include "modularity/ioc.h"
#include "async/asyncable.h"

#include "importexport/musicxml/imusicxmlconfiguration.h"
#include "importexport/guitarpro/iguitarproconfiguration.h"
#include "importexport/ove/ioveconfiguration.h"
#include "importexport/midi/imidiconfiguration.h"
#include "importexport/mei/imeiconfiguration.h"
#include "importexport/mnx/imnxconfiguration.h"
#include "notation/inotationconfiguration.h"

namespace kn::preferences {
class ImportPreferencesModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT
    QML_ELEMENT;

    Q_PROPERTY(QString styleFileImportPath READ styleFileImportPath WRITE setStyleFileImportPath NOTIFY styleFileImportPathChanged)

    Q_PROPERTY(
        QString currentOvertureCharset READ currentOvertureCharset WRITE setCurrentOvertureCharset NOTIFY currentOvertureCharsetChanged)

    Q_PROPERTY(bool importLayout READ importLayout WRITE setImportLayout NOTIFY importLayoutChanged)
    Q_PROPERTY(bool importBreaks READ importBreaks WRITE setImportBreaks NOTIFY importBreaksChanged)
    Q_PROPERTY(bool needUseDefaultFont READ needUseDefaultFont WRITE setNeedUseDefaultFont NOTIFY needUseDefaultFontChanged)
    Q_PROPERTY(bool inferTextType READ inferTextType WRITE setInferTextType NOTIFY inferTextTypeChanged)

    Q_PROPERTY(bool meiImportLayout READ meiImportLayout WRITE setMeiImportLayout NOTIFY meiImportLayoutChanged)
    Q_PROPERTY(bool mnxRequireExactSchemaValidation READ mnxRequireExactSchemaValidation WRITE setMnxRequireExactSchemaValidation
               NOTIFY mnxRequireExactSchemaValidationChanged)

    Q_PROPERTY(int currentShortestNote READ currentShortestNote WRITE setCurrentShortestNote NOTIFY currentShortestNoteChanged)
    Q_PROPERTY(bool roundTempo READ roundTempo WRITE setRoundTempo NOTIFY roundTempoChanged)

    Q_PROPERTY(
        bool needAskAboutApplyingNewStyle READ needAskAboutApplyingNewStyle WRITE setNeedAskAboutApplyingNewStyle NOTIFY needAskAboutApplyingNewStyleChanged)

    kfw::GlobalInject<iex::musicxml::IMusicXmlConfiguration> musicXmlConfiguration;
    kfw::GlobalInject<iex::guitarpro::IGuitarProConfiguration> guitarProConfiguration;
    kfw::GlobalInject<iex::ove::IOveConfiguration> oveConfiguration;
    kfw::GlobalInject<iex::midi::IMidiImportExportConfiguration> midiImportExportConfiguration;
    kfw::GlobalInject<iex::mei::IMeiConfiguration> meiConfiguration;
    kfw::GlobalInject<iex::mnxio::IMnxConfiguration> mnxConfiguration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;

public:
    explicit ImportPreferencesModel(QObject* parent = nullptr);

    Q_INVOKABLE void load();

    Q_INVOKABLE QVariantList charsets() const;
    Q_INVOKABLE QVariantList shortestNotes() const;
    Q_INVOKABLE QStringList stylePathFilter() const;
    Q_INVOKABLE QString styleChooseTitle() const;
    Q_INVOKABLE QString fileDirectory(const QString& filePath) const;

    QString styleFileImportPath() const;
    QString currentOvertureCharset() const;

    bool importLayout() const;
    bool importBreaks() const;
    bool needUseDefaultFont() const;
    bool inferTextType() const;

    int currentShortestNote() const;
    bool roundTempo() const;

    bool needAskAboutApplyingNewStyle() const;

    bool meiImportLayout() const;
    bool mnxRequireExactSchemaValidation() const;

public slots:
    void setStyleFileImportPath(QString path);
    void setCurrentOvertureCharset(QString charset);

    void setImportLayout(bool import);
    void setImportBreaks(bool import);
    void setNeedUseDefaultFont(bool value);
    void setInferTextType(bool value);

    void setCurrentShortestNote(int note);
    void setRoundTempo(bool round);

    void setNeedAskAboutApplyingNewStyle(bool value);

    void setMeiImportLayout(bool import);
    void setMnxRequireExactSchemaValidation(bool value);

signals:
    void styleFileImportPathChanged(QString styleFileImportPath);
    void currentOvertureCharsetChanged(QString currentOvertureCharset);
    void importLayoutChanged(bool importLayout);
    void importBreaksChanged(bool importBreaks);
    void needUseDefaultFontChanged(bool needUseDefaultFont);
    void inferTextTypeChanged(bool inferTextType);
    void currentShortestNoteChanged(int currentShortestNote);
    void roundTempoChanged(bool round);
    void needAskAboutApplyingNewStyleChanged(bool needAskAboutApplyingNewStyle);
    void meiImportLayoutChanged(bool importLayout);
    void mnxRequireExactSchemaValidationChanged(bool value);
};
}
