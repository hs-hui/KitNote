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
import QtQuick.Layouts

import KitNote.UiComponents
import KitNote.Ui
import KitNote.Project

// KitNote: a Column, so that the metronome option fits under the row of dropdowns.
Column {
    id: root

    property ExportDialogModel model
    property NavigationPanel navigationPanel: null
    property int navigationOrderStart: 0
    property alias navigationOrderEnd: sampleFormatsDropdown.navigation.row

    property bool showBitRateControl: false
    property bool showSampleRateControl: true

    width: parent ? parent.width : implicitWidth

    spacing: 12

    // KitNote: the widest text of a dropdown's choices (a dropdown is made as wide as it, so that a long
    // translation - "32비트 부동 소수점" - is not cut off)
    FontMetrics {
        id: dropdownFontMetrics
        font: ui.theme.bodyFont
    }

    function widestText(items) {
        let widest = 0
        for (let i = 0; i < (items ? items.length : 0); ++i) {
            widest = Math.max(widest, dropdownFontMetrics.advanceWidth(items[i].text))
        }
        return Math.ceil(widest)
    }

    // KitNote: a Flow, so that a dropdown that does not fit beside the others goes under them
    Flow {
    width: parent.width
    spacing: 12

    ExportOptionItem {
        id: sampleRateLabel
        visible: root.showSampleRateControl
        text: qsTrc("project/export", "Sample rate:")

        StyledDropdown {
            Layout.preferredWidth: 126

            navigation.name: "SampleRatesDropdown"
            navigation.panel: root.navigationPanel
            navigation.row: root.navigationOrderStart + 1
            navigation.accessible.name: sampleRateLabel.text + " " + currentText

            model: root.model ? root.model.availableSampleRates().map(function(sampleRate) {
                return { text: qsTrc("project/export", "%1 Hz").arg(sampleRate), value: sampleRate }
            }) : []

            currentIndex: root.model ? indexOfValue(root.model.sampleRate) : -1

            onActivated: function(index, value) {
                root.model.sampleRate = value
            }
        }
    }

    ExportOptionItem {
        id: bitrateLabel
        visible: root.showBitRateControl
        text: qsTrc("project/export", "Bit rate:")

        StyledDropdown {
            Layout.preferredWidth: 126

            navigation.name: "BitratesDropdown"
            navigation.panel: root.navigationPanel
            navigation.row: root.navigationOrderStart + 2
            navigation.accessible.name: bitrateLabel.text + " " + currentText

            model: root.model ? root.model.availableBitRates().map(function(bitRate) {
                return { text: qsTrc("project/export", "%1 kBit/s").arg(bitRate), value: bitRate }
            }) : []

            currentIndex: root.model ? indexOfValue(root.model.bitRate) : -1

            onActivated: function(index, value) {
                root.model.bitRate = value
            }
        }
    }

    ExportOptionItem {
        id: sampleFormatLabel
        visible: root.model ? root.model.availableSampleFormats.length > 0 : false
        text: qsTrc("project/export", "Sample format:")

        StyledDropdown {
            id: sampleFormatsDropdown

            // KitNote: the label, the arrow and their margins take about 48 (StyledDropdown)
            Layout.preferredWidth: Math.min(root.width, Math.max(126, root.widestText(model) + 48))

            navigation.name: "SampleFormatsDropdown"
            navigation.panel: root.navigationPanel
            navigation.row: root.navigationOrderStart + 3
            navigation.accessible.name: sampleFormatLabel.text + " " + currentText

            model: root.model ? root.model.availableSampleFormats : []

            currentIndex: root.model ? indexOfValue(root.model.selectedSampleFormat) : -1

            onActivated: function(index, value) {
                root.model.selectedSampleFormat = value
            }
        }
    }
    }

    CheckBox {
        width: parent.width
        text: qsTrc("project/export", "Include the metronome click")

        navigation.name: "IncludeMetronomeCheckbox"
        navigation.panel: root.navigationPanel
        navigation.row: root.navigationOrderStart + 4

        checked: root.model ? root.model.audioWithMetronome : false

        onClicked: {
            root.model.audioWithMetronome = !checked
        }
    }

    // KitNote: the recording the score plays along with (File > Play along with a recording)
    CheckBox {
        width: parent.width
        visible: root.model ? root.model.hasRecording : false
        text: qsTrc("project/export", "Include the recording")

        navigation.name: "IncludeRecordingCheckbox"
        navigation.panel: root.navigationPanel
        navigation.row: root.navigationOrderStart + 5

        checked: root.model ? root.model.audioWithRecording : false

        onClicked: {
            root.model.audioWithRecording = !checked
        }
    }

    // KitNote: at another speed the recording would not stay with the score (it is not time-stretched)
    StyledTextLabel {
        width: parent.width
        visible: root.model ? root.model.hasRecording && root.model.audioWithRecording && root.model.recordingOffSpeed : false
        text: qsTrc("project/export", "The playback speed is not 100%, so the recording is left out. Set the speed to 100% to include it.")
        horizontalAlignment: Text.AlignLeft
        wrapMode: Text.WordWrap
    }
}
