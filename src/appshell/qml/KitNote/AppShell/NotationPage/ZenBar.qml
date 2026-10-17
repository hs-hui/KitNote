/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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
pragma ComponentBehavior: Bound

import QtQuick

import KitNote.Ui
import KitNote.UiComponents
import KitNote.AppShell
import KitNote.Playback

//! KitNote: the "zen" interface. With the panels closed and no row of toolbars above it, the score
//! has the window to itself. A rail floats at its left edge with what the top row used to hold - the
//! way back to Home, note input, the panels, playback, undo - and the command search (Ctrl+K) reaches
//! everything else by name.
Item {
    id: root

    //! The score view, which gets the keyboard back when the search closes
    property var paintView: null

    //! Preferences > Appearance > Toolbar position: 0 top, 1 left, 2 right, 3 bottom
    property int position: 1

    readonly property bool horizontal: root.position === 0 || root.position === 3

    readonly property bool searchOpen: searchPanel.visible

    //! At the side, the rail is one column as long as the score view has the height for it; in a small window,
    //! or with the Mixer open below the score, it takes as many columns as it needs.
    readonly property bool railWraps: {
        if (root.horizontal) {
            return false
        }
        // its fixed buttons and dividers (see the bar below), and those of the model, 2 apart
        var height = 7 * 38 + 3 * 13
        var items = zenModel.items
        for (var i = 0; i < items.length; ++i) {
            height += items[i].divider ? 13 : 38
        }
        return height + 12 > root.height - 16
    }

    function openSearch() {
        searchPanel.visible = true
        searchField.clear()
        resultsView.model = zenModel.search("")
        resultsView.currentIndex = 0
        searchField.ensureActiveFocus()
    }

    function closeSearch() {
        searchPanel.visible = false
        if (root.paintView) {
            root.paintView.forceFocusIn()
        }
    }

    //! The cards (step grid, practice, song structure) share one place over the score: one at a time.
    function toggleCard(card) {
        var show = !card.visible
        stepGrid.visible = false
        practiceCard.visible = false
        structureCard.visible = false
        patternCard.visible = false
        card.visible = show
    }

    //! a row of the command search
    function runItem(item) {
        root.runCommand(item.code)
    }

    function runCommand(code) {
        root.closeSearch()
        // the score has the focus again when the command runs: many commands need it
        Qt.callLater(zenModel.run, code)
    }

    ZenBarModel {
        id: zenModel

        onStepGridRequested: {
            root.toggleCard(stepGrid)
        }

        onPracticeRequested: {
            root.toggleCard(practiceCard)
        }

        onPatternsRequested: {
            root.toggleCard(patternCard)
        }

        onSongStructureRequested: {
            root.toggleCard(structureCard)
        }

        onCommandSearchRequested: {
            if (root.searchOpen) {
                root.closeSearch()
            } else {
                root.openSearch()
            }
        }
    }

    PatternLibraryModel {
        id: patternModel

        watchEmptyScore: !root.starterDismissed
    }

    SongStructureModel {
        id: structureModel

        active: structureCard.visible
    }

    StepGridModel {
        id: gridModel

        active: stepGrid.visible
    }

    //! where playback is, how fast the music goes, and the practice speed
    PlaybackToolBarModel {
        id: playModel
    }

    Component.onCompleted: {
        zenModel.load()
        playModel.load()
        gridModel.load()
        structureModel.load()
        patternModel.load()
    }

    // --- the bar ---
    // An "ink" pill: the colour of the text as background, the colour of the paper for the icons,
    // so that it stands out from the page in the light and in the dark theme alike.
    component BarButton: Rectangle {
        id: button

        property int icon: IconCode.NONE
        property string label: ""
        property string hint: ""
        property string title: ""
        property string shortcut: ""
        property bool checked: false

        signal clicked()

        width: Math.max(36, buttonRow.implicitWidth + (button.label !== "" ? 24 : 0))
        height: 36
        radius: height / 2

        opacity: enabled ? 1.0 : 0.35

        color: button.checked ? ui.theme.accentColor
                              : buttonMouse.pressed ? Qt.alpha(bar.inkOnBar, 0.26)
                                                    : buttonMouse.containsMouse ? Qt.alpha(bar.inkOnBar, 0.14) : "transparent"

        Row {
            id: buttonRow

            anchors.centerIn: parent
            spacing: 7

            StyledIconLabel {
                anchors.verticalCenter: parent.verticalCenter
                iconCode: button.icon
                visible: button.icon !== IconCode.NONE
                color: button.checked ? ui.theme.fontPrimaryColor : bar.inkOnBar
            }

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter
                text: button.label
                visible: button.label !== ""
                color: bar.inkOnBar
            }

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter
                text: button.hint
                visible: button.hint !== ""
                color: bar.inkOnBar
                opacity: 0.55
            }
        }

        MouseArea {
            id: buttonMouse

            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                ui.tooltip.hide(button, true)
                button.clicked()
            }

            onContainsMouseChanged: {
                if (containsMouse && button.title !== "") {
                    ui.tooltip.show(button, button.title, "", button.shortcut)
                } else {
                    ui.tooltip.hide(button)
                }
            }
        }
    }

    component BarDivider: Item {
        // (in a rail of several columns the groups do not stay together: no dividers)
        visible: !root.railWraps
        width: root.horizontal ? 11 : 36
        height: root.horizontal ? 36 : 11

        Rectangle {
            anchors.centerIn: parent
            width: root.horizontal ? 1 : 18
            height: root.horizontal ? 18 : 1
            color: Qt.alpha(bar.inkOnBar, 0.25)
        }
    }

    StyledRectangularShadow {
        anchors.fill: bar
        visible: bar.visible
        z: bar.z
        blur: 18
        radius: bar.radius
    }

    Rectangle {
        id: bar

        readonly property color inkOnBar: ui.theme.backgroundPrimaryColor

        // along the middle of its edge; clear of the scroll bars at the right and at the bottom
        x: root.horizontal ? Math.round((parent.width - width) / 2)
                           : (root.position === 2 ? parent.width - width - 22 : 14)
        y: !root.horizontal ? Math.round((parent.height - height) / 2)
                            : (root.position === 3 ? parent.height - height - 26 : 14)

        width: root.horizontal ? barColumn.width + 12 : Math.max(48, barColumn.width + 12)
        height: root.horizontal ? 48 : barColumn.height + 12
        radius: 24

        color: ui.theme.fontPrimaryColor

        // clicks on the rail do not fall through to the score
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
        }

        Grid {
            id: barColumn

            anchors.centerIn: parent
            spacing: 2

            // one row along the top or the bottom, one column at the sides (or several: railWraps)
            flow: root.horizontal ? Grid.LeftToRight : Grid.TopToBottom
            rows: {
                if (root.horizontal) {
                    return 1
                }
                if (!root.railWraps) {
                    return -1
                }
                // the buttons (no dividers then) in as few columns as fit, as even as can be
                var buttons = 7
                for (var i = 0; i < zenModel.items.length; ++i) {
                    buttons += zenModel.items[i].divider ? 0 : 1
                }
                var fitting = Math.max(4, Math.floor((root.height - 40) / 38))
                return Math.ceil(buttons / Math.ceil(buttons / fitting))
            }
            columns: root.horizontal ? -1 : (root.railWraps ? -1 : 1)

            verticalItemAlignment: Grid.AlignVCenter
            horizontalItemAlignment: Grid.AlignHCenter

            BarButton {
                icon: IconCode.ARROW_LEFT
                title: qsTrc("appshell", "Back to the scores")

                onClicked: {
                    api.launcher.open("kitnote://home")
                }
            }

            BarDivider {}

            Repeater {
                model: zenModel.items

                Loader {
                    id: railItem

                    required property var modelData

                    visible: !(railItem.modelData.divider && root.railWraps)

                    sourceComponent: railItem.modelData.divider ? dividerComponent : buttonComponent

                    Component {
                        id: dividerComponent

                        BarDivider {}
                    }

                    Component {
                        id: buttonComponent

                        BarButton {
                            // "play" is checked while the music plays, and then stops it
                            icon: railItem.modelData.code === "play" && railItem.modelData.checked
                                  ? IconCode.PAUSE : railItem.modelData.icon
                            title: railItem.modelData.title
                            shortcut: railItem.modelData.shortcut
                            checked: railItem.modelData.checked
                            enabled: railItem.modelData.enabled

                            onClicked: {
                                zenModel.dispatch(railItem.modelData.code)
                            }
                        }
                    }
                }
            }

            BarButton {
                icon: IconCode.GRID
                title: qsTrc("appshell", "Step grid")
                checked: stepGrid.visible

                onClicked: {
                    root.toggleCard(stepGrid)
                }
            }

            BarButton {
                icon: IconCode.MEASURE_REPEAT
                title: qsTrc("appshell", "Pattern library")
                checked: patternCard.visible

                onClicked: {
                    root.toggleCard(patternCard)
                }
            }

            BarButton {
                icon: IconCode.CLOCK
                title: qsTrc("appshell", "Practice with rising tempo")
                checked: practiceCard.visible || practice.active

                onClicked: {
                    root.toggleCard(practiceCard)
                }
            }

            BarButton {
                icon: IconCode.LIST
                title: qsTrc("appshell", "Song structure")
                checked: structureCard.visible

                onClicked: {
                    root.toggleCard(structureCard)
                }
            }

            BarDivider {}

            // measure.beat over the tempo; a click steps the practice speed 100 - 75 - 50 %
            Rectangle {
                id: tempoChip

                readonly property int percent: Math.round(playModel.tempoMultiplier * 100)
                readonly property string positionText: playModel.measureNumber + "." + playModel.beatNumber
                readonly property string tempoText: playModel.tempo.value + (tempoChip.percent !== 100 ? " ×" + tempoChip.percent + "%" : "")

                width: root.horizontal ? chipRow.implicitWidth + 20 : 36
                height: 36
                radius: 18

                color: tempoChip.percent !== 100 ? ui.theme.accentColor
                                                 : chipMouse.containsMouse ? Qt.alpha(bar.inkOnBar, 0.14) : "transparent"

                readonly property color ink: tempoChip.percent !== 100 ? ui.theme.fontPrimaryColor : bar.inkOnBar

                Row {
                    id: chipRow

                    anchors.centerIn: parent
                    spacing: 8
                    visible: root.horizontal

                    StyledTextLabel {
                        text: tempoChip.positionText
                        color: tempoChip.ink
                        opacity: 0.7
                    }

                    StyledTextLabel {
                        text: tempoChip.tempoText
                        color: tempoChip.ink
                        font: ui.theme.bodyBoldFont
                    }
                }

                Column {
                    anchors.centerIn: parent
                    spacing: -2
                    visible: !root.horizontal

                    StyledTextLabel {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: tempoChip.positionText
                        color: tempoChip.ink
                        opacity: 0.7
                        font.pixelSize: 10
                    }

                    StyledTextLabel {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: tempoChip.percent !== 100 ? tempoChip.percent + "%" : playModel.tempo.value
                        color: tempoChip.ink
                        font.pixelSize: 11
                        font.bold: true
                    }
                }

                MouseArea {
                    id: chipMouse

                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        ui.tooltip.hide(tempoChip, true)
                        playModel.tempoMultiplier = tempoChip.percent > 75 ? 0.75 : (tempoChip.percent > 50 ? 0.5 : 1.0)
                    }

                    onContainsMouseChanged: {
                        if (containsMouse) {
                            // KitNote: the recording of the score is not time-stretched
                            ui.tooltip.show(tempoChip, qsTrc("appshell", "Position and tempo"),
                                            qsTrc("appshell", "Click to practise at 75% or 50% of the tempo")
                                            + (playModel.hasRecording ? "\n" + qsTrc("appshell", "The recording is heard at 100% only.") : ""), "")
                        } else {
                            ui.tooltip.hide(tempoChip)
                        }
                    }
                }
            }

            BarDivider {}

            BarButton {
                icon: IconCode.SEARCH
                title: qsTrc("appshell", "Find a command")
                shortcut: "Ctrl+K"
                checked: root.searchOpen

                onClicked: {
                    if (root.searchOpen) {
                        root.closeSearch()
                    } else {
                        root.openSearch()
                    }
                }
            }
        }
    }

    // --- the step grid ---
    // The selected measure as a drum machine shows it: a row for every drum, a step for every sixteenth
    // note. A click puts a drum on a step or takes it off, and the measure is written again.
    StyledRectangularShadow {
        anchors.fill: stepGrid
        visible: stepGrid.visible
        blur: 24
        radius: stepGrid.radius
    }

    Rectangle {
        id: stepGrid

        readonly property var familyColors: ["#B9B6AC", "#D98B6A", "#8DB3A4", "#E3B95B", "#D9A441", "#8FA6C4"]
        readonly property int cell: 22
        readonly property int nameWidth: 132

        // at the foot of the score until it is dragged somewhere else
        x: Math.round((parent.width - width) / 2)
        y: parent.height - height - (root.position === 3 ? 88 : 30)

        width: Math.min(parent.width - 140, gridContent.implicitWidth + 32)
        height: gridContent.implicitHeight + 28
        radius: 12

        color: ui.theme.popupBackgroundColor
        border.width: 1
        border.color: ui.theme.strokeColor

        visible: false

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons

            // the card is moved by its background
            drag.target: stepGrid

            // from the first drag on the card stays where it is put: its place no longer follows its size
            onPressed: {
                stepGrid.x = stepGrid.x
                stepGrid.y = stepGrid.y
            }
            drag.minimumX: 0
            drag.maximumX: root.width - stepGrid.width
            drag.minimumY: 0
            drag.maximumY: root.height - stepGrid.height
            cursorShape: pressed ? Qt.ClosedHandCursor : Qt.ArrowCursor
        }

        Column {
            id: gridContent

            anchors.centerIn: parent
            spacing: 6

            Item {
                width: Math.max(titleLabel2.implicitWidth + 110, gridRows.visible ? gridRows.width : 420)
                height: 24

                StyledTextLabel {
                    id: titleLabel2

                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    font: ui.theme.bodyBoldFont
                    text: gridModel.available
                          ? qsTrc("appshell", "Step grid") + "  ·  " + qsTrc("appshell", "measure %1").arg(gridModel.measureNumber)
                          : qsTrc("appshell", "Step grid")
                }

                Row {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 2

                    FlatButton {
                        width: 24
                        height: 24
                        icon: IconCode.ARROW_LEFT
                        transparent: true
                        enabled: gridModel.available
                        toolTipTitle: qsTrc("appshell", "Previous measure")

                        onClicked: {
                            gridModel.goToMeasure(false)
                        }
                    }

                    FlatButton {
                        width: 24
                        height: 24
                        icon: IconCode.ARROW_RIGHT
                        transparent: true
                        enabled: gridModel.available
                        toolTipTitle: qsTrc("appshell", "Next measure")

                        onClicked: {
                            gridModel.goToMeasure(true)
                        }
                    }

                    FlatButton {
                        width: 24
                        height: 24
                        icon: IconCode.CLOSE_X_ROUNDED
                        transparent: true

                        onClicked: {
                            stepGrid.visible = false
                        }
                    }
                }
            }

            StyledTextLabel {
                width: 420
                height: 48
                wrapMode: Text.WordWrap
                opacity: 0.7
                visible: !gridModel.available || !gridModel.fitsGrid || gridModel.repeatSign

                text: !gridModel.available
                      ? qsTrc("appshell", "Select a measure of a drum staff to see it here.")
                      : gridModel.repeatSign
                        ? qsTrc("appshell", "This measure is a repeat sign. Write it out first (Tools) to edit its steps.")
                        : qsTrc("appshell", "This measure has notes between the sixteenths (triplets or shorter notes), so it cannot be shown as steps.")
            }

            Row {
                visible: gridRows.visible
                spacing: 12

                StyledTextLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    opacity: 0.55
                    text: qsTrc("appshell", "Click: stroke on or off.  Right-click: accent, ghost note, plain.")
                }

                // an empty measure can be a grid of triplets (a shuffle, a 12/8 feel in 4/4)
                FlatButton {
                    height: 24
                    isNarrow: true
                    enabled: gridModel.empty
                    accentButton: gridModel.triplets
                    text: qsTrc("appshell", "Triplets")
                    toolTipTitle: qsTrc("appshell", "Three steps to a beat. For an empty measure.")

                    onClicked: {
                        gridModel.setTriplets(!gridModel.triplets)
                    }
                }

                FlatButton {
                    height: 24
                    isNarrow: true
                    text: qsTrc("appshell", "Copy to next measure")

                    onClicked: {
                        zenModel.dispatch("repeat-sel")
                        gridModel.goToMeasure(true)
                    }
                }

                FlatButton {
                    height: 24
                    isNarrow: true
                    text: qsTrc("appshell", "Clear")

                    onClicked: {
                        zenModel.dispatch("action://notation/delete")
                    }
                }
            }

            Column {
                id: gridRows

                spacing: 3
                visible: gridModel.available && gridModel.fitsGrid && !gridModel.repeatSign

                // the beats
                Row {
                    spacing: 3

                    Item {
                        width: stepGrid.nameWidth
                        height: 14
                    }

                    Repeater {
                        model: gridModel.stepCount

                        Item {
                            required property int index

                            width: stepGrid.cell + (index % gridModel.stepsPerBeat === gridModel.stepsPerBeat - 1 ? 7 : 0)
                            height: 14

                            StyledTextLabel {
                                anchors.left: parent.left
                                width: stepGrid.cell
                                opacity: 0.55
                                font.pixelSize: 10
                                text: parent.index % gridModel.stepsPerBeat === 0 ? (parent.index / gridModel.stepsPerBeat + 1) : ""
                            }

                            // the playhead: a bar over the step that sounds
                            Rectangle {
                                anchors.left: parent.left
                                anchors.bottom: parent.bottom
                                width: stepGrid.cell
                                height: 3
                                radius: 1.5
                                visible: gridModel.playStep === parent.index
                                color: ui.theme.accentColor
                            }
                        }
                    }
                }

                Repeater {
                    model: gridModel.rows

                    Row {
                        id: gridRow

                        required property var modelData

                        spacing: 3

                        StyledTextLabel {
                            width: stepGrid.nameWidth
                            height: stepGrid.cell
                            horizontalAlignment: Text.AlignRight
                            rightPadding: 8
                            text: gridRow.modelData.name
                        }

                        Repeater {
                            model: gridRow.modelData.steps

                            Item {
                                id: stepItem

                                required property var modelData
                                required property int index

                                width: stepGrid.cell + (index % gridModel.stepsPerBeat === gridModel.stepsPerBeat - 1 ? 7 : 0)
                                height: stepGrid.cell

                                // a stroke is filled; an accent is filled and marked; a ghost note is an outline
                                Rectangle {
                                    id: stepCell

                                    readonly property int state: stepItem.modelData
                                    readonly property color familyColor: stepGrid.familyColors[gridRow.modelData.family]

                                    width: stepGrid.cell
                                    height: stepGrid.cell
                                    radius: 5

                                    color: stepCell.state === 1 || stepCell.state === 2
                                           ? stepCell.familyColor
                                           : stepCell.state === 3 ? Qt.alpha(stepCell.familyColor, 0.25)
                                                                  : Qt.alpha(ui.theme.fontPrimaryColor, stepMouse.containsMouse ? 0.2 : 0.08)
                                    border.width: stepCell.state === 3 ? 2 : 0
                                    border.color: stepCell.familyColor

                                    // the step that sounds lights up
                                    Rectangle {
                                        anchors.fill: parent
                                        radius: parent.radius
                                        visible: gridModel.playStep === stepItem.index
                                        color: Qt.alpha(ui.theme.fontPrimaryColor, stepCell.state === 0 ? 0.14 : 0.35)
                                    }

                                    StyledTextLabel {
                                        anchors.centerIn: parent
                                        visible: stepCell.state === 2
                                        color: "#2B2B28"
                                        font.bold: true
                                        text: ">"
                                    }

                                    MouseArea {
                                        id: stepMouse

                                        anchors.fill: parent
                                        hoverEnabled: true
                                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                                        onClicked: function(mouse) {
                                            if (mouse.button === Qt.RightButton) {
                                                gridModel.cycleKind(gridRow.modelData.pitch, stepItem.index)
                                            } else {
                                                gridModel.toggle(gridRow.modelData.pitch, stepItem.index)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // --- practice ---
    // The selection (or the whole score) is looped, slowly at first and a little faster with every pass,
    // until the tempo of the score is reached: the way a hard measure is learned.
    QtObject {
        id: practice

        property bool active: false
        property int startPercent: 60
        property int stepPercent: 5
        property int targetPercent: 100
        property int passes: 0
        property real lastPosition: 0
        property bool wasPlaying: false

        readonly property int percent: Math.round(playModel.tempoMultiplier * 100)

        function actionState(code) {
            for (var i = 0; i < zenModel.items.length; ++i) {
                if (zenModel.items[i].code === code) {
                    return zenModel.items[i]
                }
            }
            return null
        }

        function start() {
            practice.passes = 0
            practice.lastPosition = playModel.playPosition
            playModel.tempoMultiplier = Math.min(practice.startPercent, practice.targetPercent) / 100

            var loop = practice.actionState("loop")
            if (loop && !loop.checked) {
                zenModel.dispatch("loop")       // around the selection, when there is one
            }

            practice.wasPlaying = false
            practice.active = true

            if (root.paintView) {
                root.paintView.forceFocusIn()   // "play" needs the score to have the keyboard
            }
            Qt.callLater(function() {
                var play = practice.actionState("play")
                if (play && !play.checked) {
                    zenModel.dispatch("play")
                }
            })
        }

        function stop() {
            practice.active = false

            var play = practice.actionState("play")
            if (play && play.checked) {
                zenModel.dispatch("play")
            }

            playModel.tempoMultiplier = 1.0
        }
    }

    Connections {
        target: playModel
        enabled: practice.active

        // the position jumps back when the loop starts again: one pass done
        function onPlayPositionChanged() {
            var position = playModel.playPosition
            var jumpedBack = position < practice.lastPosition - 0.0005
            practice.lastPosition = position    // first: changing the tempo may report the position again

            var play = practice.actionState("play")
            if (jumpedBack && play && play.checked) {
                practice.passes += 1
                var next = Math.min(practice.targetPercent, practice.percent + practice.stepPercent)
                if (next !== practice.percent) {
                    playModel.tempoMultiplier = next / 100
                }
            }
        }
    }

    Connections {
        target: zenModel
        enabled: practice.active

        // playback ended (the user stopped it, or there was no loop and the score came to its end):
        // the practice is over, and the tempo goes back
        function onItemsChanged() {
            var play = practice.actionState("play")
            if (!play) {
                return
            }
            if (play.checked) {
                practice.wasPlaying = true
            } else if (practice.wasPlaying) {
                practice.active = false
                playModel.tempoMultiplier = 1.0
            }
        }
    }

    Component.onDestruction: {
        if (practice.active) {
            playModel.tempoMultiplier = 1.0
        }
    }

    StyledRectangularShadow {
        anchors.fill: practiceCard
        visible: practiceCard.visible
        blur: 24
        radius: practiceCard.radius
    }

    Rectangle {
        id: practiceCard

        // at the foot of the score until it is dragged somewhere else
        x: Math.round((parent.width - width) / 2)
        y: parent.height - height - (root.position === 3 ? 88 : 30)

        width: practiceContent.implicitWidth + 36
        height: practiceContent.implicitHeight + 32
        radius: 12

        color: ui.theme.popupBackgroundColor
        border.width: 1
        border.color: ui.theme.strokeColor

        visible: false

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons

            // the card is moved by its background
            drag.target: practiceCard

            // from the first drag on the card stays where it is put: its place no longer follows its size
            onPressed: {
                practiceCard.x = practiceCard.x
                practiceCard.y = practiceCard.y
            }
            drag.minimumX: 0
            drag.maximumX: root.width - practiceCard.width
            drag.minimumY: 0
            drag.maximumY: root.height - practiceCard.height
            cursorShape: pressed ? Qt.ClosedHandCursor : Qt.ArrowCursor
        }

        Column {
            id: practiceContent

            anchors.centerIn: parent
            spacing: 12

            Item {
                width: practiceFields.width
                height: 24

                StyledTextLabel {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    font: ui.theme.bodyBoldFont
                    text: qsTrc("appshell", "Practice with rising tempo")
                }

                FlatButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: 24
                    height: 24
                    icon: IconCode.CLOSE_X_ROUNDED
                    transparent: true

                    onClicked: {
                        practiceCard.visible = false
                    }
                }
            }

            Row {
                id: practiceFields

                spacing: 16

                Repeater {
                    model: [
                        { key: "startPercent", label: qsTrc("appshell", "Start at"), min: 30, max: 100, step: 5 },
                        { key: "stepPercent", label: qsTrc("appshell", "Faster every pass by"), min: 1, max: 25, step: 1 },
                        { key: "targetPercent", label: qsTrc("appshell", "Up to"), min: 50, max: 150, step: 5 }
                    ]

                    Column {
                        id: field

                        required property var modelData

                        spacing: 6

                        StyledTextLabel {
                            opacity: 0.7
                            text: field.modelData.label
                        }

                        IncrementalPropertyControl {
                            width: 108

                            enabled: !practice.active
                            currentValue: practice[field.modelData.key]
                            minValue: field.modelData.min
                            maxValue: field.modelData.max
                            step: field.modelData.step
                            decimals: 0
                            measureUnitsSymbol: "%"

                            onValueEdited: function(newValue) {
                                practice[field.modelData.key] = Math.round(newValue)
                            }
                        }
                    }
                }
            }

            Row {
                spacing: 14

                FlatButton {
                    width: 132
                    accentButton: !practice.active
                    text: practice.active ? qsTrc("appshell", "Stop") : qsTrc("appshell", "Start")

                    onClicked: {
                        if (practice.active) {
                            practice.stop()
                        } else {
                            practice.start()
                        }
                    }
                }

                StyledTextLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignLeft
                    opacity: 0.7

                    text: practice.active
                          ? qsTrc("appshell", "Pass %1 at %2% of the tempo").arg(practice.passes + 1).arg(practice.percent)
                          : qsTrc("appshell", "Select the measures to loop, then start.")
                }
            }

            SeparatorLine {}

            // Tap tempo: the beat tapped on the button, as quarter notes a minute; "Use" writes it into
            // the score as the tempo of the music.
            Row {
                id: tapRow

                property var taps: []
                property int bpm: 0

                function tap() {
                    const now = Date.now()
                    var taps = tapRow.taps
                    // a pause starts a new count
                    if (taps.length > 0 && now - taps[taps.length - 1] > 2000) {
                        taps = []
                    }
                    taps.push(now)
                    if (taps.length > 8) {
                        taps.shift()
                    }
                    tapRow.taps = taps
                    if (taps.length >= 2) {
                        tapRow.bpm = Math.round(60000 * (taps.length - 1) / (taps[taps.length - 1] - taps[0]))
                    }
                }

                spacing: 14

                FlatButton {
                    width: 132
                    text: qsTrc("appshell", "Tap the tempo")

                    // at the press, not at the release: the beat is where the finger lands
                    mouseArea.onPressed: {
                        tapRow.tap()
                    }
                }

                StyledTextLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 76
                    horizontalAlignment: Text.AlignLeft
                    font: ui.theme.bodyBoldFont
                    opacity: tapRow.bpm > 0 ? 1.0 : 0.5
                    text: tapRow.bpm > 0 ? tapRow.bpm + " BPM" : "— BPM"
                }

                FlatButton {
                    enabled: tapRow.bpm >= 20 && tapRow.bpm <= 400
                    text: qsTrc("appshell", "Use as the tempo of the score")

                    onClicked: {
                        structureModel.setMusicTempo(tapRow.bpm)
                    }
                }
            }
        }
    }

    // --- the song structure ---
    // The sections of the chart as a list: the name, where it starts and how long it is. A click goes
    // there; the names at the bottom start a section at the selected measure.
    StyledRectangularShadow {
        anchors.fill: structureCard
        visible: structureCard.visible
        blur: 24
        radius: structureCard.radius
    }

    Rectangle {
        id: structureCard

        // at the foot of the score until it is dragged somewhere else
        x: Math.round((parent.width - width) / 2)
        y: parent.height - height - (root.position === 3 ? 88 : 30)

        width: 520
        height: structureContent.implicitHeight + 32
        radius: 12

        color: ui.theme.popupBackgroundColor
        border.width: 1
        border.color: ui.theme.strokeColor

        visible: false

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons

            // the card is moved by its background
            drag.target: structureCard

            // from the first drag on the card stays where it is put: its place no longer follows its size
            onPressed: {
                structureCard.x = structureCard.x
                structureCard.y = structureCard.y
            }
            drag.minimumX: 0
            drag.maximumX: root.width - structureCard.width
            drag.minimumY: 0
            drag.maximumY: root.height - structureCard.height
            cursorShape: pressed ? Qt.ClosedHandCursor : Qt.ArrowCursor
        }

        Column {
            id: structureContent

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 18
            anchors.verticalCenter: parent.verticalCenter

            spacing: 10

            Item {
                width: parent.width
                height: 24

                StyledTextLabel {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    font: ui.theme.bodyBoldFont
                    text: qsTrc("appshell", "Song structure")
                }

                FlatButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: 24
                    height: 24
                    icon: IconCode.CLOSE_X_ROUNDED
                    transparent: true

                    onClicked: {
                        structureCard.visible = false
                    }
                }
            }

            StyledTextLabel {
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.WordWrap
                opacity: 0.7
                visible: sectionsView.count === 0
                text: qsTrc("appshell", "No sections yet. Select a measure and choose a name below to start one there.")
            }

            ListView {
                id: sectionsView

                width: parent.width
                height: Math.min(contentHeight, 7 * 32)
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                visible: count > 0

                model: structureModel.sections

                delegate: Rectangle {
                    id: sectionRow

                    required property var modelData
                    required property int index

                    width: ListView.view.width
                    height: 32
                    radius: 6

                    color: sectionRow.modelData.current ? Qt.alpha(ui.theme.accentColor, ui.theme.accentOpacityNormal)
                                                        : sectionMouse.containsMouse ? Qt.alpha(ui.theme.buttonColor, ui.theme.buttonOpacityHover)
                                                                                     : "transparent"

                    StyledTextLabel {
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.right: sectionInfo.left
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignLeft
                        font: ui.theme.bodyBoldFont
                        text: sectionRow.modelData.name
                    }

                    StyledTextLabel {
                        id: sectionInfo

                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        opacity: 0.6
                        text: qsTrc("appshell", "from measure %1 · %n measure(s)", "", sectionRow.modelData.measureCount)
                              .arg(sectionRow.modelData.firstMeasure)
                    }

                    MouseArea {
                        id: sectionMouse

                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            structureModel.goTo(sectionRow.index)
                        }
                    }
                }
            }

            StyledTextLabel {
                opacity: 0.55
                visible: sectionsView.count > 0
                text: qsTrc("appshell", "Fill the section of the selected measure with:")
            }

            Flow {
                width: parent.width
                spacing: 6
                visible: sectionsView.count > 0

                Repeater {
                    model: structureModel.grooves()

                    FlatButton {
                        required property var modelData

                        height: 28
                        isNarrow: true
                        text: modelData.title

                        onClicked: {
                            structureModel.fillCurrentSection(modelData.pattern, fillAtEndBox.checked)
                        }
                    }
                }
            }

            CheckBox {
                id: fillAtEndBox

                visible: sectionsView.count > 0
                checked: true
                text: qsTrc("appshell", "End the section with a fill")

                onClicked: {
                    checked = !checked
                }
            }

            StyledTextLabel {
                opacity: 0.55
                text: qsTrc("appshell", "Start a section at the selected measure:")
            }

            Flow {
                width: parent.width
                spacing: 6

                Repeater {
                    model: ["Intro", "Verse", "Pre-Chorus", "Chorus", "Bridge", "Solo", "Interlude", "Outro"]

                    FlatButton {
                        required property string modelData

                        height: 28
                        isNarrow: true
                        text: modelData

                        onClicked: {
                            structureModel.addSection(modelData)
                        }
                    }
                }
            }
        }
    }

    // --- a word of welcome on an empty score ---
    // Three ways to begin. It goes away with the first note, with one of its buttons, or when it is closed.
    property bool starterDismissed: false

    Connections {
        target: patternModel

        function onScoreOpened() {
            root.starterDismissed = false
        }
    }

    StyledRectangularShadow {
        anchors.fill: starter
        visible: starter.visible
        blur: 18
        radius: starter.radius
    }

    Rectangle {
        id: starter

        anchors.horizontalCenter: parent.horizontalCenter
        y: root.position === 0 ? 76 : 18

        width: starterRow.implicitWidth + 28
        height: 48
        radius: 24

        color: ui.theme.popupBackgroundColor
        border.width: 1
        border.color: ui.theme.strokeColor

        visible: patternModel.scoreEmpty && !root.starterDismissed && !root.searchOpen
                 && !stepGrid.visible && !patternCard.visible && !practiceCard.visible && !structureCard.visible

        Row {
            id: starterRow

            anchors.centerIn: parent
            spacing: 8

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter
                rightPadding: 6
                text: qsTrc("appshell", "An empty score. Begin with:")
            }

            FlatButton {
                height: 30
                isNarrow: true
                text: qsTrc("appshell", "Note input")

                onClicked: {
                    root.starterDismissed = true
                    zenModel.dispatch("first-element")
                    zenModel.dispatch("note-input")
                }
            }

            FlatButton {
                height: 30
                isNarrow: true
                text: qsTrc("appshell", "Step grid")

                onClicked: {
                    zenModel.dispatch("first-element")
                    root.toggleCard(stepGrid)
                }
            }

            FlatButton {
                height: 30
                isNarrow: true
                text: qsTrc("appshell", "A groove in every measure")

                onClicked: {
                    zenModel.dispatch("notation-select-all")
                    root.toggleCard(patternCard)
                }
            }

            FlatButton {
                width: 24
                height: 24
                anchors.verticalCenter: parent.verticalCenter
                icon: IconCode.CLOSE_X_ROUNDED
                transparent: true

                onClicked: {
                    root.starterDismissed = true
                }
            }
        }
    }

    // --- the pattern library ---
    // Grooves, fills and rudiments, each as a small step grid of one measure. A click writes the
    // pattern into the selected measures (it is the command of "Add > Drum patterns").
    StyledRectangularShadow {
        anchors.fill: patternCard
        visible: patternCard.visible
        blur: 24
        radius: patternCard.radius
    }

    Rectangle {
        id: patternCard

        property int group: 0

        // a pattern that was clicked and could not be written anywhere
        property bool notWritten: false

        onGroupChanged: {
            notWritten = false
        }

        readonly property int tileWidth: 196

        // at the foot of the score until it is dragged somewhere else
        x: Math.round((parent.width - width) / 2)
        y: parent.height - height - (root.position === 3 ? 88 : 30)

        width: Math.min(parent.width - 40, tileWidth * 3 + 8 * 2 + 36)
        height: patternContent.implicitHeight + 32
        radius: 12

        color: ui.theme.popupBackgroundColor
        border.width: 1
        border.color: ui.theme.strokeColor

        visible: false

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons

            // the card is moved by its background
            drag.target: patternCard

            // from the first drag on the card stays where it is put: its place no longer follows its size
            onPressed: {
                patternCard.x = patternCard.x
                patternCard.y = patternCard.y
            }
            drag.minimumX: 0
            drag.maximumX: root.width - patternCard.width
            drag.minimumY: 0
            drag.maximumY: root.height - patternCard.height
            cursorShape: pressed ? Qt.ClosedHandCursor : Qt.ArrowCursor
        }

        Column {
            id: patternContent

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 18
            anchors.verticalCenter: parent.verticalCenter

            spacing: 10

            Item {
                width: parent.width
                height: 28

                StyledTextLabel {
                    id: patternTitle

                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    font: ui.theme.bodyBoldFont
                    text: qsTrc("appshell", "Pattern library")
                }

                Row {
                    anchors.left: patternTitle.right
                    anchors.leftMargin: 18
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 4

                    Repeater {
                        model: [qsTrc("appshell", "Grooves"), qsTrc("appshell", "Fills"), qsTrc("appshell", "Rudiments"), qsTrc("appshell", "My patterns")]

                        Rectangle {
                            id: groupTab

                            required property string modelData
                            required property int index

                            readonly property bool current: patternCard.group === index

                            width: groupLabel.implicitWidth + 24
                            height: 26
                            radius: 13

                            color: current ? ui.theme.accentColor
                                           : Qt.alpha(ui.theme.fontPrimaryColor, groupMouse.containsMouse ? 0.12 : 0.0)

                            StyledTextLabel {
                                id: groupLabel

                                anchors.centerIn: parent
                                text: groupTab.modelData
                                font: groupTab.current ? ui.theme.bodyBoldFont : ui.theme.bodyFont
                                color: groupTab.current ? "#1F2A26" : ui.theme.fontPrimaryColor
                            }

                            MouseArea {
                                id: groupMouse

                                anchors.fill: parent
                                hoverEnabled: true

                                onClicked: {
                                    patternCard.group = groupTab.index
                                }
                            }
                        }
                    }
                }

                FlatButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: 24
                    height: 24
                    icon: IconCode.CLOSE_X_ROUNDED
                    transparent: true

                    onClicked: {
                        patternCard.visible = false
                    }
                }
            }

            StyledTextLabel {
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                opacity: 0.55
                text: qsTrc("appshell", "Select measures, then click a pattern to write it there.")
            }

            StyledTextLabel {
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.WordWrap
                visible: patternCard.notWritten
                color: ui.theme.accentColor
                text: qsTrc("appshell", "Not written: select a measure of the length of the pattern (the random fills are in 4/4).")
            }

            // "Fills": one made up on the spot - another one at every click
            Row {
                visible: patternCard.group === 1
                spacing: 8

                FlatButton {
                    text: qsTrc("appshell", "Random fill: a measure")

                    onClicked: {
                        patternCard.notWritten = patternModel.writeRandomFill(false) === 0
                    }
                }

                FlatButton {
                    text: qsTrc("appshell", "Random fill: half a measure")

                    onClicked: {
                        patternCard.notWritten = patternModel.writeRandomFill(true) === 0
                    }
                }

                StyledTextLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    opacity: 0.55
                    text: qsTrc("appshell", "Click again for another one.")
                }
            }

            // "My patterns": keeps the selected measure
            Row {
                visible: patternCard.group === 3
                spacing: 8

                TextInputField {
                    id: patternNameField

                    width: 170
                    hint: qsTrc("appshell", "Name (optional)")

                    onTextChanged: function(newTextValue) {
                        patternNameField.currentText = newTextValue
                    }
                }

                FlatButton {
                    height: patternNameField.height
                    text: qsTrc("appshell", "Keep the selected measure")

                    onClicked: {
                        if (patternModel.saveSelectedMeasure(patternNameField.currentText)) {
                            patternNameField.currentText = ""
                            patternSaveHint.visible = false
                        } else {
                            patternSaveHint.visible = true
                        }
                    }
                }

                StyledTextLabel {
                    id: patternSaveHint

                    anchors.verticalCenter: parent.verticalCenter
                    visible: false
                    opacity: 0.8
                    text: qsTrc("appshell", "Select a measure with drum notes first.")
                }
            }

            StyledTextLabel {
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                opacity: 0.55
                visible: patternCard.group === 3 && patternModel.userPatterns.length === 0
                text: qsTrc("appshell", "Nothing here yet. A measure you keep can be written into any score.")
            }

            StyledFlickable {
                id: patternFlick

                width: parent.width
                // the score stays in sight: the selection decides where a pattern goes
                height: Math.min(patternFlow.height, Math.max(120, Math.min(250, root.height * 0.36)))

                contentWidth: width
                contentHeight: patternFlow.height
                clip: true

                Flow {
                    id: patternFlow

                    width: parent.width
                    spacing: 8

                    Repeater {
                        // read while the card is shown
                        model: !patternCard.visible ? [] : patternCard.group === 3 ? patternModel.userPatterns
                                                                                  : patternModel.patterns(patternCard.group)

                        Rectangle {
                            id: tile

                            required property var modelData
                            required property int index

                            readonly property int cell: modelData.steps > 16 ? 5 : modelData.steps > 12 ? 8 : 10
                            readonly property int gap: 4

                            width: patternCard.tileWidth
                            height: tileColumn.implicitHeight + 20
                            radius: 8

                            color: Qt.alpha(ui.theme.fontPrimaryColor, tileMouse.pressed ? 0.16 : tileMouse.containsMouse ? 0.10 : 0.05)
                            border.width: 1
                            border.color: tileMouse.containsMouse ? ui.theme.accentColor : "transparent"

                            Column {
                                id: tileColumn

                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.margins: 10
                                anchors.verticalCenter: parent.verticalCenter

                                spacing: 8

                                StyledTextLabel {
                                    width: parent.width
                                    horizontalAlignment: Text.AlignLeft
                                    text: tile.modelData.title
                                    font: ui.theme.bodyBoldFont
                                }

                                Column {
                                    spacing: 2

                                    Repeater {
                                        model: tile.modelData.rows

                                        Row {
                                            id: tileRow

                                            required property var modelData

                                            spacing: 0

                                            StyledTextLabel {
                                                width: 22
                                                height: tile.cell
                                                horizontalAlignment: Text.AlignLeft
                                                text: tileRow.modelData.label
                                                font.pixelSize: 9
                                                opacity: 0.6
                                            }

                                            Repeater {
                                                model: tileRow.modelData.states

                                                Item {
                                                    required property int modelData
                                                    required property int index

                                                    // a little more room before every beat
                                                    width: tile.cell + 1 + (index > 0 && index % tile.modelData.group === 0 ? tile.gap : 0)
                                                    height: tile.cell

                                                    Rectangle {
                                                        anchors.right: parent.right
                                                        anchors.rightMargin: 1
                                                        width: tile.cell
                                                        height: tile.cell
                                                        radius: 2

                                                        color: parent.modelData > 0 ? stepGrid.familyColors[tileRow.modelData.family]
                                                                                    : Qt.alpha(ui.theme.fontPrimaryColor, 0.09)
                                                        opacity: parent.modelData === 1 && tile.modelData.sticking !== "" ? 0.6 : 1.0
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // the sticking of a rudiment under its strokes
                                    Row {
                                        visible: tile.modelData.sticking !== ""
                                        spacing: 0

                                        Item {
                                            width: 22
                                            height: 12
                                        }

                                        Repeater {
                                            model: tile.modelData.sticking.length

                                            Item {
                                                required property int index

                                                width: tile.cell + 1 + (index > 0 && index % tile.modelData.group === 0 ? tile.gap : 0)
                                                height: 12

                                                StyledTextLabel {
                                                    anchors.right: parent.right
                                                    anchors.rightMargin: 1
                                                    width: tile.cell
                                                    text: tile.modelData.sticking.charAt(parent.index)
                                                    font.pixelSize: 9
                                                    opacity: 0.75
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            MouseArea {
                                id: tileMouse

                                anchors.fill: parent
                                hoverEnabled: true

                                onClicked: {
                                    if (tile.modelData.user) {
                                        patternCard.notWritten = patternModel.applyUserPattern(tile.index) === 0
                                    } else {
                                        Qt.callLater(zenModel.run, tile.modelData.code)
                                    }
                                }
                            }

                            // a pattern of one's own can be thrown away
                            FlatButton {
                                anchors.top: parent.top
                                anchors.right: parent.right
                                anchors.margins: 4
                                width: 20
                                height: 20
                                visible: tile.modelData.user === true
                                icon: IconCode.CLOSE_X_ROUNDED
                                transparent: true

                                onClicked: {
                                    patternModel.removeUserPattern(tile.index)
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // --- the command search ---
    MouseArea {
        // a click anywhere else closes the search
        anchors.fill: parent
        visible: root.searchOpen
        acceptedButtons: Qt.AllButtons

        onPressed: {
            root.closeSearch()
        }
    }

    StyledRectangularShadow {
        anchors.fill: searchPanel
        visible: searchPanel.visible
        blur: 28
        radius: searchPanel.radius
    }

    Rectangle {
        id: searchPanel

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: Math.max(76, parent.height * 0.12)      // below the rail when that is at the top

        width: Math.min(520, parent.width - 48)
        height: searchField.height + 24 + (resultsView.count > 0 ? resultsView.height + 12 : emptyLabel.height + 12)

        radius: 12
        color: ui.theme.popupBackgroundColor
        border.width: 1
        border.color: ui.theme.strokeColor

        visible: false

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
        }

        FocusScope {
            anchors.fill: parent
            anchors.margins: 12

            Keys.onUpPressed: {
                resultsView.currentIndex = Math.max(0, resultsView.currentIndex - 1)
            }

            Keys.onDownPressed: {
                resultsView.currentIndex = Math.min(resultsView.count - 1, resultsView.currentIndex + 1)
            }

            SearchField {
                id: searchField

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                hint: qsTrc("appshell", "Find a command")

                onSearchTextChanged: {
                    if (!root.searchOpen) {
                        return
                    }
                    resultsView.model = zenModel.search(searchField.searchText)
                    resultsView.currentIndex = 0
                }

                onAccepted: {
                    const item = resultsView.model[resultsView.currentIndex]
                    if (item) {
                        root.runItem(item)
                    }
                }

                onEscaped: {
                    root.closeSearch()
                }
            }

            StyledTextLabel {
                id: emptyLabel

                anchors.top: searchField.bottom
                anchors.topMargin: 12
                anchors.left: parent.left
                anchors.right: parent.right

                height: 36
                opacity: 0.6
                visible: resultsView.count === 0
                text: qsTrc("appshell", "No command with that name")
            }

            ListView {
                id: resultsView

                anchors.top: searchField.bottom
                anchors.topMargin: 12
                anchors.left: parent.left
                anchors.right: parent.right

                height: Math.min(contentHeight, 9 * 36)
                clip: true
                boundsBehavior: Flickable.StopAtBounds

                onCurrentIndexChanged: {
                    positionViewAtIndex(currentIndex, ListView.Contain)
                }

                delegate: Rectangle {
                    id: row

                    required property var modelData
                    required property int index

                    width: ListView.view.width
                    height: 36
                    radius: 4

                    readonly property bool current: ListView.isCurrentItem

                    color: row.current ? Qt.alpha(ui.theme.accentColor, ui.theme.accentOpacityNormal)
                                       : rowMouse.containsMouse ? Qt.alpha(ui.theme.buttonColor, ui.theme.buttonOpacityHover)
                                                                : "transparent"

                    StyledIconLabel {
                        id: recentMark

                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        width: 16
                        iconCode: row.modelData.recent ? IconCode.UNDO : IconCode.NONE
                        opacity: 0.5
                    }

                    StyledTextLabel {
                        id: titleLabel

                        anchors.left: recentMark.right
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter

                        width: Math.min(implicitWidth, row.width - shortcutLabel.width - 60)
                        horizontalAlignment: Text.AlignLeft
                        text: row.modelData.title
                    }

                    // what the command does, when its name does not already say it
                    StyledTextLabel {
                        anchors.left: titleLabel.right
                        anchors.leftMargin: 10
                        anchors.right: shortcutLabel.left
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        horizontalAlignment: Text.AlignLeft
                        opacity: 0.5
                        text: row.modelData.description
                    }

                    StyledTextLabel {
                        id: shortcutLabel

                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        opacity: 0.6
                        text: row.modelData.shortcut
                    }

                    MouseArea {
                        id: rowMouse

                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            root.runItem(row.modelData)
                        }
                    }
                }
            }
        }
    }
}
