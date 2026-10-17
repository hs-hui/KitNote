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

#include <QAbstractListModel>
#include <qqmlintegration.h>
#include <QQmlParserStatus>

#include "app_config.h"

#include "modularity/ioc.h"

#include "async/asyncable.h"

#include "iinteractive.h"
#include "context/iglobalcontext.h"
#include "importexport/imagesexport/iimagesexportconfiguration.h"
#include "importexport/musicxml/imusicxmlconfiguration.h"
#include "importexport/midi/imidiconfiguration.h"
#include "importexport/audioexport/iaudioexportconfiguration.h"
#include "importexport/mei/imeiconfiguration.h"
#include "importexport/lyricsexport/ilyricsexportconfiguration.h"
#include "importexport/mnx/imnxconfiguration.h"

#include "iexportprojectscenario.h"
#include "inotationwritersregister.h"
#include "iprojectconfiguration.h"

class QItemSelectionModel;

namespace kn::project {
class ExportDialogModel : public QAbstractListModel, public QQmlParserStatus, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int selectionLength READ selectionLength NOTIFY selectionChanged)

    Q_PROPERTY(QVariantMap selectedExportType READ selectedExportType NOTIFY selectedExportTypeChanged)

    Q_PROPERTY(QVariantList availableUnitTypes READ availableUnitTypes NOTIFY selectedExportTypeChanged)
    Q_PROPERTY(int selectedUnitType READ selectedUnitType WRITE setUnitType NOTIFY selectedUnitTypeChanged)

    Q_PROPERTY(int pdfResolution READ pdfResolution WRITE setPdfResolution NOTIFY pdfResolutionChanged)
    Q_PROPERTY(
        bool pdfTransparentBackground READ pdfTransparentBackground WRITE setPdfTransparentBackground NOTIFY pdfTransparentBackgroundChanged)

    Q_PROPERTY(bool pdfGrayscale READ pdfGrayscale WRITE setPdfGrayscale NOTIFY pdfGrayscaleChanged)

    Q_PROPERTY(int pngResolution READ pngResolution WRITE setPngResolution NOTIFY pngResolutionChanged)
    Q_PROPERTY(
        bool pngTransparentBackground READ pngTransparentBackground WRITE setPngTransparentBackground NOTIFY pngTransparentBackgroundChanged)
    Q_PROPERTY(bool pngGrayscale READ pngGrayscale WRITE setPngGrayscale NOTIFY pngGrayscaleChanged)

    Q_PROPERTY(
        bool svgTransparentBackground READ svgTransparentBackground WRITE setSvgTransparentBackground NOTIFY svgTransparentBackgroundChanged)
    Q_PROPERTY(bool svgIllustratorCompat READ svgIllustratorCompat WRITE setSvgIllustratorCompat NOTIFY svgIllustratorCompatChanged FINAL)



    Q_PROPERTY(int sampleRate READ sampleRate WRITE setSampleRate NOTIFY sampleRateChanged)
    Q_PROPERTY(int bitRate READ bitRate WRITE setBitRate NOTIFY bitRateChanged)
    Q_PROPERTY(bool audioWithMetronome READ audioWithMetronome WRITE setAudioWithMetronome NOTIFY audioWithMetronomeChanged) // KitNote
    Q_PROPERTY(bool hasRecording READ hasRecording CONSTANT) // KitNote
    Q_PROPERTY(bool recordingOffSpeed READ recordingOffSpeed CONSTANT) // KitNote
    Q_PROPERTY(bool audioWithRecording READ audioWithRecording WRITE setAudioWithRecording NOTIFY audioWithRecordingChanged) // KitNote
    Q_PROPERTY(QVariantList availableSampleFormats READ availableSampleFormats NOTIFY availableSampleFormatsChanged)
    Q_PROPERTY(int selectedSampleFormat READ selectedSampleFormat WRITE setSelectedSampleFormat NOTIFY selectedSampleFormatChanged)

    Q_PROPERTY(bool midiExpandRepeats READ midiExpandRepeats WRITE setMidiExpandRepeats NOTIFY midiExpandRepeatsChanged)
    Q_PROPERTY(bool midiExportRpns READ midiExportRpns WRITE setMidiExportRpns NOTIFY midiExportRpnsChanged)

    Q_PROPERTY(MusicXmlLayoutType musicXmlLayoutType READ musicXmlLayoutType WRITE setMusicXmlLayoutType NOTIFY musicXmlLayoutTypeChanged)

    Q_PROPERTY(int meiExportLayout READ meiExportLayout WRITE setMeiExportLayout NOTIFY meiExportLayoutChanged)
    Q_PROPERTY(int meiUseKitNoteIds READ meiUseKitNoteIds WRITE setMeiUseKitNoteIds NOTIFY meiUseKitNoteIdsChanged)

    Q_PROPERTY(int lrcUseEnhancedFormat READ lrcUseEnhancedFormat WRITE setLrcUseEnhancedFormat NOTIFY lrcUseEnhancedFormatChanged)

    Q_PROPERTY(int mnxIndentSpaces READ mnxIndentSpaces WRITE setMnxIndentSpaces NOTIFY mnxIndentSpacesChanged)
    Q_PROPERTY(bool mnxExportBeams READ mnxExportBeams WRITE setMnxExportBeams NOTIFY mnxExportBeamsChanged)
    Q_PROPERTY(bool mnxExportRestPositions READ mnxExportRestPositions WRITE setMnxExportRestPositions
               NOTIFY mnxExportRestPositionsChanged)

    Q_PROPERTY(bool shouldDestinationFolderBeOpenedOnExport READ shouldDestinationFolderBeOpenedOnExport
               WRITE setShouldDestinationFolderBeOpenedOnExport NOTIFY shouldDestinationFolderBeOpenedOnExportChanged)


    QML_ELEMENT

    kfw::GlobalInject<iex::musicxml::IMusicXmlConfiguration> musicXmlConfiguration;
    kfw::GlobalInject<iex::midi::IMidiImportExportConfiguration> midiImportExportConfiguration;
    kfw::GlobalInject<iex::audioexport::IAudioExportConfiguration> audioExportConfiguration;
    kfw::GlobalInject<iex::mei::IMeiConfiguration> meiConfiguration;
    kfw::GlobalInject<iex::lrcexport::ILyricsExportConfiguration> lrcConfiguration;
    kfw::GlobalInject<iex::mnxio::IMnxConfiguration> mnxConfiguration;
    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::GlobalInject<iex::imagesexport::IImagesExportConfiguration> imageExportConfiguration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<context::IGlobalContext> context = { this };
    kfw::ContextInject<INotationWritersRegister> writers = { this };
    kfw::ContextInject<IExportProjectScenario> exportProjectScenario = { this };

public:
    explicit ExportDialogModel(QObject* parent = nullptr);
    ~ExportDialogModel() override;

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void setSelected(int scoreIndex, bool selected = true);
    Q_INVOKABLE void setAllSelected(bool selected);
    Q_INVOKABLE void selectCurrentNotation();
    int selectionLength() const;

    Q_INVOKABLE QVariantList exportTypeList() const;
    QVariantMap selectedExportType() const;
    void setExportType(const ExportType& type);
    Q_INVOKABLE void selectExportTypeById(const QString& id);

    QVariantList availableUnitTypes() const;
    int selectedUnitType() const;
    void setUnitType(int unitType);
    void setUnitType(project::INotationWriter::UnitType unitType);

    Q_INVOKABLE bool exportScores();

    int pdfResolution() const;
    void setPdfResolution(const int& resolution);

    bool pdfTransparentBackground() const;
    void setPdfTransparentBackground(const bool& transparent);

    bool pdfGrayscale() const;
    void setPdfGrayscale(const bool& grayscale);

    int pngResolution() const;
    void setPngResolution(const int& resolution);

    bool pngTransparentBackground() const;
    void setPngTransparentBackground(const bool& transparent);

    bool pngGrayscale() const;
    void setPngGrayscale(const bool& grayscale);

    bool svgTransparentBackground() const;
    void setSvgTransparentBackground(const bool& transparent);

    bool svgIllustratorCompat() const;
    void setSvgIllustratorCompat(bool compat);



    Q_INVOKABLE QList<int> availableSampleRates() const;
    int sampleRate() const;
    void setSampleRate(int sampleRate);

    Q_INVOKABLE QList<int> availableBitRates() const;
    int bitRate() const;
    void setBitRate(int bitRate);

    bool audioWithMetronome() const;
    void setAudioWithMetronome(bool withMetronome);

    bool hasRecording() const;
    bool recordingOffSpeed() const;
    bool audioWithRecording() const;
    void setAudioWithRecording(bool withRecording);

    QVariantList availableSampleFormats() const;
    int selectedSampleFormat() const;
    void setSelectedSampleFormat(int format);

    bool midiExpandRepeats() const;
    void setMidiExpandRepeats(bool expandRepeats);

    bool midiExportRpns() const;
    void setMidiExportRpns(bool exportRpns);

    bool meiExportLayout() const;
    void setMeiExportLayout(bool exportLayout);

    bool meiUseKitNoteIds() const;
    void setMeiUseKitNoteIds(bool useKitNoteIds);

    bool lrcUseEnhancedFormat() const;
    void setLrcUseEnhancedFormat(bool useEnhancedFormat);

    int mnxIndentSpaces() const;
    void setMnxIndentSpaces(int spaces);

    bool mnxExportBeams() const;
    void setMnxExportBeams(bool exportBeams);

    bool mnxExportRestPositions() const;
    void setMnxExportRestPositions(bool exportRestPositions);

    enum class MusicXmlLayoutType {
        AllLayout,
        AllBreaks,
        ManualBreaks,
        None
    };
    Q_ENUM(MusicXmlLayoutType)

    Q_INVOKABLE QVariantList musicXmlLayoutTypes() const;
    MusicXmlLayoutType musicXmlLayoutType() const;
    void setMusicXmlLayoutType(MusicXmlLayoutType layoutType);

    bool shouldDestinationFolderBeOpenedOnExport() const;
    void setShouldDestinationFolderBeOpenedOnExport(bool enabled);


    Q_INVOKABLE void updateExportInfo();

signals:
    void selectionChanged();

    void selectedExportTypeChanged(QVariantMap newExportType);
    void selectedUnitTypeChanged(project::INotationWriter::UnitType newUnitType);

    void pdfResolutionChanged(int resolution);
    void pdfTransparentBackgroundChanged(bool transparent);
    void pdfGrayscaleChanged(bool grayscale);

    void pngResolutionChanged(int resolution);
    void pngTransparentBackgroundChanged(bool transparent);
    void pngGrayscaleChanged(bool grayscale);

    void svgTransparentBackgroundChanged(bool transparent);
    void svgIllustratorCompatChanged(bool compat);


    void availableSampleRatesChanged();
    void sampleRateChanged(int sampleRate);
    void availableBitRatesChanged();
    void bitRateChanged(int bitRate);
    void audioWithMetronomeChanged();
    void audioWithRecordingChanged();
    void availableSampleFormatsChanged();
    void selectedSampleFormatChanged();

    void midiExpandRepeatsChanged(bool expandRepeats);
    void midiExportRpnsChanged(bool exportRpns);

    void musicXmlLayoutTypeChanged(MusicXmlLayoutType layoutType);

    void meiExportLayoutChanged(bool exportLayout);
    void meiUseKitNoteIdsChanged(bool useKitNoteIds);

    void lrcUseEnhancedFormatChanged(bool enhancedFormat);

    void mnxIndentSpacesChanged(int spaces);
    void mnxExportBeamsChanged(bool exportBeams);
    void mnxExportRestPositionsChanged(bool exportRestPositions);

    void shouldDestinationFolderBeOpenedOnExportChanged(bool shouldDestinationFolderBeOpenedOnExport);


private:
    void classBegin() override;
    void componentComplete() override {}
    void init();

    enum Roles {
        RoleTitle = Qt::UserRole + 1,
        RoleIsSelected,
        RoleIsMain
    };

    bool isIndexValid(int index) const;

    bool isFormatSelected(const QString& formatSuffix) const;

    bool isMainNotation(notation::INotationPtr notation) const;
    notation::IMasterNotationPtr masterNotation() const;

    void selectSavedNotations();


    QList<notation::INotationPtr> m_notations {};
    QItemSelectionModel* m_selectionModel = nullptr;

    ExportTypeList m_exportTypeList {};
    ExportType m_selectedExportType = ExportType();
    kfw::io::path_t m_exportPath;
    project::INotationWriter::UnitType m_selectedUnitType = project::INotationWriter::UnitType::PER_PART;
};
}
