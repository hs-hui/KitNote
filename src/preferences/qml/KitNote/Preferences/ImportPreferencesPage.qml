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
import QtQuick

import KitNote.UiComponents
import KitNote.Preferences

import "internal"

PreferencesPage {
    id: root

    Component.onCompleted: {
        importPreferencesModel.load()
    }

    ImportPreferencesModel {
        id: importPreferencesModel
    }

    Column {
        width: parent.width
        spacing: root.sectionsSpacing

        ImportStyleSection {
            id: importStyleSection
            styleFileImportPath: importPreferencesModel.styleFileImportPath
            fileChooseTitle: importPreferencesModel.styleChooseTitle()
            filePathFilter: importPreferencesModel.stylePathFilter()
            fileDirectory: importPreferencesModel.fileDirectory(styleFileImportPath)

            navigation.section: root.navigationSection
            navigation.order: root.navigationOrderStart + 1

            onStyleFileImportPathChangeRequested: function(path) {
                importPreferencesModel.styleFileImportPath = path
            }

            onFocusChanged: {
                if (activeFocus) {
                    root.ensureContentVisibleRequested(Qt.rect(x, y, width, height))
                }
            }
        }

        SeparatorLine { }

        CharsetsSection {
            // KitNote: this importer is not built.
            visible: false

            charsets: importPreferencesModel.charsets()
            currentOvertureCharset: importPreferencesModel.currentOvertureCharset

            navigation.section: root.navigationSection
            navigation.order: root.navigationOrderStart + 2

            onOvertureCharsetChangeRequested: function(charset) {
                importPreferencesModel.currentOvertureCharset = charset
            }

            onFocusChanged: {
                if (activeFocus) {
                    root.ensureContentVisibleRequested(Qt.rect(x, y, width, height))
                }
            }
        }

        SeparatorLine { visible: false }

        MusicXmlSection {
            importLayout: importPreferencesModel.importLayout
            importBreaks: importPreferencesModel.importBreaks
            needUseDefaultFont: importPreferencesModel.needUseDefaultFont
            inferTextType: importPreferencesModel.inferTextType

            navigation.section: root.navigationSection
            navigation.order: root.navigationOrderStart + 3

            onImportLayoutChangeRequested: function(importLayout) {
                importPreferencesModel.importLayout = importLayout
            }

            onImportBreaksChangeRequested: function(importBreaks) {
                importPreferencesModel.importBreaks = importBreaks
            }

            onUseDefaultFontChangeRequested: function(use) {
                importPreferencesModel.needUseDefaultFont = use
            }

            onInferTextTypeChangeRequested: function (inferTextType) {
                importPreferencesModel.inferTextType = inferTextType
            }

            onFocusChanged: {
                if (activeFocus) {
                    root.ensureContentVisibleRequested(Qt.rect(x, y, width, height))
                }
            }
        }

        SeparatorLine { }

        MidiSection {
            shortestNotes: importPreferencesModel.shortestNotes()
            currentShortestNote: importPreferencesModel.currentShortestNote

            roundTempo: importPreferencesModel.roundTempo

            navigation.section: root.navigationSection
            navigation.order: root.navigationOrderStart + 4

            onCurrentShortestNoteChangeRequested: function(note) {
                importPreferencesModel.currentShortestNote = note
            }

            onRoundTempoChangeRequested: function(round) {
                importPreferencesModel.roundTempo = round
            }

            onFocusChanged: {
                if (activeFocus) {
                    root.ensureContentVisibleRequested(Qt.rect(x, y, width, height))
                }
            }
        }

        SeparatorLine { visible: false }

        MeiSection {
            // KitNote: this importer is not built.
            visible: false

            meiImportLayout: importPreferencesModel.meiImportLayout

            onMeiImportLayoutChangeRequested: function(meiImportLayout) {
                importPreferencesModel.meiImportLayout = meiImportLayout
            }
        }

        SeparatorLine { visible: false }

        MnxSection {
            // KitNote: this importer is not built.
            visible: false

            requireExactSchemaValidation: importPreferencesModel.mnxRequireExactSchemaValidation

            navigation.section: root.navigationSection
            navigation.order: root.navigationOrderStart + 5

            onRequireExactSchemaValidationChangeRequested: function(value) {
                importPreferencesModel.mnxRequireExactSchemaValidation = value
            }

            onFocusChanged: {
                if (activeFocus) {
                    root.ensureContentVisibleRequested(Qt.rect(x, y, width, height))
                }
            }
        }
    }

    function reset() {
        importStyleSection.reset()
    }
}
