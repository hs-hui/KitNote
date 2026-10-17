/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

pragma ComponentBehavior: Bound

import QtQuick

import KitNote.Ui
import KitNote.UiComponents
import KitNote.Dock
import KitNote.AppShell

import KitNote.NotationScene
import KitNote.Palette
import KitNote.Inspector
import KitNote.InstrumentsScene
import KitNote.Playback

DockPage {
    id: root

    objectName: "Notation"
    uri: "kitnote://notation"

    required property NavigationSection topToolbarKeyNavSec

    property NotationPageModel pageModel: NotationPageModel {}

    //! NOTE KitNote: read by the dock window, which then leaves the row of page tabs out of this page
    readonly property bool kitnoteNoTopToolBars: true

    property NavigationSection noteInputKeyNavSec: NavigationSection {
        name: "NoteInputSection"
        order: 2
    }

    property NavigationSection keynavTopPanelSec: NavigationSection {
        name: "NavigationTopPanel"
        enabled: root.visible
        order: 3
    }

    property NavigationSection keynavLeftPanelSec: NavigationSection {
        name: "NavigationLeftPanel"
        enabled: root.visible
        order: 4
    }

    property NavigationSection keynavRightPanelSec: NavigationSection {
        name: "NavigationRightPanel"
        enabled: root.visible
        order: 6
    }

    property NavigationSection keynavBottomPanelSec: NavigationSection {
        name: "NavigationBottomPanel"
        enabled: root.visible
        order: 7
    }

    function navigationPanelSec(location) {
        switch(location) {
        case Location.Top: return keynavTopPanelSec
        case Location.Left: return keynavLeftPanelSec
        case Location.Right: return keynavRightPanelSec
        case Location.Bottom: return keynavBottomPanelSec
        }

        return null
    }

    onInited: {
        Qt.callLater(pageModel.init)
    }

    readonly property int verticalPanelDefaultWidth: 300

    readonly property int horizontalPanelMinHeight: 100
    readonly property int horizontalPanelMaxHeight: 520

    readonly property int panelMinDimension: 10
    readonly property int panelMaxDimension: 7500 //! NOTE: Value found experimentally - see issue #27770

    readonly property string verticalPanelsGroup: "VERTICAL_PANELS"
    readonly property string horizontalPanelsGroup: "HORIZONTAL_PANELS"

    readonly property var verticalPanelDropDestinations: [
        { "dock": root.centralDock, "dropLocation": Location.Left, "dropDistance": root.verticalPanelDefaultWidth },
        { "dock": root.centralDock, "dropLocation": Location.Right, "dropDistance": root.verticalPanelDefaultWidth }
    ]

    readonly property var horizontalPanelDropDestinations: [
        root.panelTopDropDestination,
        root.panelBottomDropDestination
    ]

    property var notationView: null

    mainToolBars: [
        DockToolBar {
            id: notationToolBar

            objectName: "notationToolBar"
            title: qsTrc("appshell", "Notation toolbar")

            visible: false // KitNote: the mixer is on the rail

            floatable: false
            closable: false
            resizable: false
            separatorsVisible: false

            alignment: DockToolBarAlignment.Center
            contentBottomPadding: 2

            navigationSection: root.topToolbarKeyNavSec

            NotationToolBar {
                navigationPanel.section: notationToolBar.navigationSection
                navigationPanel.order: 2
            }
        },

        DockToolBar {
            id: playbackToolBar

            objectName: root.pageModel.playbackToolBarName()
            title: qsTrc("appshell", "Playback controls")

            visible: false // KitNote: playback is on the rail; View > Toolbars brings this back

            separatorsVisible: false
            alignment: DockToolBarAlignment.Right
            resizable: !floating

            contentBottomPadding: floating ? 8 : 2
            contentTopPadding: floating ? 8 : 0

            dropDestinations: [
                { "dock": notationToolBar, "dropLocation": Location.Right }
            ]

            navigationSection: root.topToolbarKeyNavSec

            PlaybackToolBar {
                navigationPanelSection: playbackToolBar.navigationSection
                navigationPanelOrder: 3

                floating: playbackToolBar.floating
            }
        },

        DockToolBar {
            id: undoRedoToolBar

            objectName: root.pageModel.undoRedoToolBarName()
            title: qsTrc("appshell", "Undo/redo")

            visible: false // KitNote: undo and redo are on the rail

            floatable: false
            closable: false
            resizable: false
            separatorsVisible: false

            alignment: DockToolBarAlignment.Right
            contentBottomPadding: 2

            navigationSection: root.topToolbarKeyNavSec

            UndoRedoToolBar {
                navigationPanel.section: undoRedoToolBar.navigationSection
                navigationPanel.order: 5
            }
        }
    ]

    toolBars: [
        DockToolBar {
            id: noteInputBar

            objectName: root.pageModel.noteInputBarName()
            title: qsTrc("appshell", "Note input")

            //! NOTE KitNote: the "zen" layout starts with the score alone; the floating bar brings this back
            visible: false

            dropDestinations: [
                root.toolBarTopDropDestination,
                root.toolBarBottomDropDestination,
                root.toolBarLeftDropDestination,
                root.toolBarRightDropDestination
            ]

            thickness: orientation === Qt.Horizontal ? 40 : 76
            resizable: !floating

            navigationSection: root.noteInputKeyNavSec

            NoteInputBar {
                orientation: noteInputBar.orientation
                floating: noteInputBar.floating

                maximumWidth: noteInputBar.width
                maximumHeight: noteInputBar.height

                navigationPanel.section: noteInputBar.navigationSection
                navigationPanel.order: 1
            }
        }
    ]

    panels: [
        DockPanel {
            id: palettesPanel

            objectName: root.pageModel.palettesPanelName()
            title: qsTrc("appshell", "Palettes")

            visible: false // KitNote: "zen" layout

            navigationSection: root.navigationPanelSec(palettesPanel.location)

            width: root.verticalPanelDefaultWidth
            minimumWidth: root.verticalPanelDefaultWidth
            maximumWidth: root.verticalPanelDefaultWidth

            minimumHeight: root.panelMinDimension
            maximumHeight: root.panelMaxDimension

            groupName: root.verticalPanelsGroup

            dropDestinations: root.verticalPanelDropDestinations

            PalettesPanel {
                navigationSection: palettesPanel.navigationSection
                navigationOrderStart: palettesPanel.contentNavigationPanelOrderStart

                Component.onCompleted: {
                    palettesPanel.contextMenuModel = contextMenuModel
                }
            }
        },

        DockPanel {
            id: layoutPanel

            objectName: root.pageModel.layoutPanelName()

            visible: false // KitNote: "zen" layout
            title: qsTrc("appshell", "Layout")

            navigationSection: root.navigationPanelSec(layoutPanel.location)

            width: root.verticalPanelDefaultWidth
            minimumWidth: root.verticalPanelDefaultWidth
            maximumWidth: root.verticalPanelDefaultWidth

            minimumHeight: root.panelMinDimension
            maximumHeight: root.panelMaxDimension

            groupName: root.verticalPanelsGroup

            dropDestinations: root.verticalPanelDropDestinations

            LayoutPanel {
                navigationSection: layoutPanel.navigationSection
                navigationOrderStart: layoutPanel.contentNavigationPanelOrderStart

                Component.onCompleted: {
                    layoutPanel.contextMenuModel = contextMenuModel
                }
            }
        },

        DockPanel {
            id: inspectorPanel

            objectName: root.pageModel.inspectorPanelName()
            title: qsTrc("appshell", "Properties")

            visible: false // KitNote: "zen" layout

            navigationSection: root.navigationPanelSec(inspectorPanel.location)

            width: root.verticalPanelDefaultWidth
            minimumWidth: root.verticalPanelDefaultWidth
            maximumWidth: root.verticalPanelDefaultWidth

            minimumHeight: root.panelMinDimension
            maximumHeight: root.panelMaxDimension

            groupName: root.verticalPanelsGroup

            dropDestinations: root.verticalPanelDropDestinations

            InspectorForm {
                navigationSection: inspectorPanel.navigationSection
                navigationOrderStart: inspectorPanel.contentNavigationPanelOrderStart
                notationView: root.notationView
            }
        },

        DockPanel {
            id: selectionFilterPanel

            objectName: root.pageModel.selectionFiltersPanelName()
            title: qsTrc("appshell", "Selection filter")

            navigationSection: root.navigationPanelSec(selectionFilterPanel.location)

            width: root.verticalPanelDefaultWidth
            minimumWidth: root.verticalPanelDefaultWidth
            maximumWidth: root.verticalPanelDefaultWidth

            minimumHeight: root.panelMinDimension
            maximumHeight: root.panelMaxDimension

            groupName: root.verticalPanelsGroup

            //! NOTE: hidden by default
            visible: false

            dropDestinations: root.verticalPanelDropDestinations

            SelectionFilterPanel {
                navigationSection: selectionFilterPanel.navigationSection
                navigationOrderStart: selectionFilterPanel.contentNavigationPanelOrderStart
            }
        },

        DockPanel {
            id: undoHistoryPanel

            objectName: root.pageModel.undoHistoryPanelName()
            title: qsTrc("notation", "History")

            navigationSection: root.navigationPanelSec(undoHistoryPanel.location)

            width: root.verticalPanelDefaultWidth
            minimumWidth: root.verticalPanelDefaultWidth
            maximumWidth: root.verticalPanelDefaultWidth

            minimumHeight: root.panelMinDimension
            maximumHeight: root.panelMaxDimension

            groupName: root.verticalPanelsGroup
            location: Location.Right

            //! NOTE: hidden by default
            visible: false

            dropDestinations: root.verticalPanelDropDestinations

            UndoHistoryPanel {
                navigationSection: undoHistoryPanel.navigationSection
                navigationOrderStart: undoHistoryPanel.contentNavigationPanelOrderStart
            }
        },
        
        // =============================================
        // Horizontal Panels
        // =============================================

        DockPanel {
            id: mixerPanel

            objectName: root.pageModel.mixerPanelName()
            title: qsTrc("appshell", "Mixer")

            height: 368
            minimumHeight: root.horizontalPanelMinHeight
            maximumHeight: root.horizontalPanelMaxHeight

            minimumWidth: root.panelMinDimension
            maximumWidth: root.panelMaxDimension

            groupName: root.horizontalPanelsGroup

            //! NOTE: hidden by default
            visible: false

            location: Location.Bottom

            dropDestinations: root.horizontalPanelDropDestinations

            navigationSection: root.navigationPanelSec(mixerPanel.location)

            MixerPanel {
                id: mixerPanelComponent

                navigationSection: mixerPanel.navigationSection
                contentNavigationPanelOrderStart: mixerPanel.contentNavigationPanelOrderStart

                Component.onCompleted: {
                    mixerPanel.contextMenuModel = contextMenuModel
                    mixerPanel.toolbarComponent = toolbarComponent
                }

                Component.onDestruction: {
                    mixerPanel.contextMenuModel = null
                    mixerPanel.toolbarComponent = null
                }

                onResizeRequested: function(newWidth, newHeight) {
                    mixerPanel.resize(newWidth, newHeight)
                }

                Connections {
                    target: mixerPanel
                    function onPanelShown() {
                        mixerPanelComponent.resizePanelToContentHeight()
                    }
                }
            }
        },

        DockPanel {
            id: pianoKeyboardPanel

            objectName: root.pageModel.pianoKeyboardPanelName()
            title: qsTrc("appshell", "Piano keyboard")

            height: 200
            minimumHeight: root.horizontalPanelMinHeight
            maximumHeight: root.horizontalPanelMaxHeight

            minimumWidth: root.panelMinDimension
            maximumWidth: root.panelMaxDimension

            groupName: root.horizontalPanelsGroup

            //! NOTE: hidden by default
            visible: false

            location: Location.Bottom

            dropDestinations: root.horizontalPanelDropDestinations

            navigationSection: root.navigationPanelSec(pianoKeyboardPanel.location)

            PianoKeyboardPanel {
                navigationSection: pianoKeyboardPanel.navigationSection
                contentNavigationPanelOrderStart: pianoKeyboardPanel.contentNavigationPanelOrderStart

                Component.onCompleted: {
                    pianoKeyboardPanel.contextMenuModel = contextMenuModel
                }
            }
        },

        DockPanel {
            id: timelinePanel

            objectName: root.pageModel.timelinePanelName()
            title: qsTrc("appshell", "Timeline")

            height: 200
            minimumHeight: root.horizontalPanelMinHeight
            maximumHeight: root.horizontalPanelMaxHeight

            minimumWidth: root.panelMinDimension
            maximumWidth: root.panelMaxDimension

            groupName: root.horizontalPanelsGroup

            //! NOTE: hidden by default
            visible: false

            location: Location.Bottom

            dropDestinations: root.horizontalPanelDropDestinations

            navigationSection: root.navigationPanelSec(timelinePanel.location)

            Timeline {
                navigationSection: timelinePanel.navigationSection
                contentNavigationPanelOrderStart: timelinePanel.contentNavigationPanelOrderStart
            }
        },

        DockPanel {
            id: drumsetPanel

            objectName: root.pageModel.drumsetPanelName()
            title: qsTrc("appshell", "Drumset tools")

            height: 64
            minimumHeight: 64
            maximumHeight: 64

            //! NOTE: hidden by default
            visible: false

            floatable: false
            closable: false

            location: Location.Bottom

            navigationSection: root.navigationPanelSec(drumsetPanel.location)

            DrumsetPanel {
                navigationSection: drumsetPanel.navigationSection
                contentNavigationPanelOrderStart: drumsetPanel.contentNavigationPanelOrderStart
            }
        },

        DockPanel {
            id: percussionPanel

            objectName: root.pageModel.percussionPanelName()
            title: qsTrc("appshell", "Percussion")

            height: 200
            minimumHeight: root.horizontalPanelMinHeight
            maximumHeight: root.horizontalPanelMaxHeight

            minimumWidth: root.panelMinDimension
            maximumWidth: root.panelMaxDimension

            groupName: root.horizontalPanelsGroup

            //! NOTE KitNote: closed at first, like the other panels (the "zen" layout). It opens by itself
            //! when note input starts, and the floating bar opens and closes it.
            visible: false

            location: Location.Bottom

            dropDestinations: root.horizontalPanelDropDestinations

            navigationSection: root.navigationPanelSec(percussionPanel.location)

            PercussionPanel {
                id: percussionComponent

                navigationSection: percussionPanel.navigationSection
                contentNavigationPanelOrderStart: percussionPanel.contentNavigationPanelOrderStart

                Component.onCompleted: {
                    percussionPanel.toolbarComponent = toolbarComponent
                }

                Component.onDestruction: {
                    percussionPanel.toolbarComponent = null
                }

                onResizeRequested: function(newWidth, newHeight) {
                    percussionPanel.resize(newWidth, newHeight)
                }

                Connections {
                    target: percussionPanel
                    function onPanelShown() {
                        percussionComponent.resizePanelToContentHeight()
                    }
                }
            }
        }
    ]

    central: NotationView {
        id: notationView
        name: "MainNotationView"

        isNavigatorVisible: root.pageModel.isNavigatorVisible
        isBraillePanelVisible: root.pageModel.isBraillePanelVisible
        isMainView: true

        Component.onCompleted: {
            root.notationView = notationView.paintView

            root.setDefaultNavigationControl(notationView.defaultNavigationControl)
        }

        Component.onDestruction: {
            root.setDefaultNavigationControl(null)
        }

        //! NOTE KitNote: the floating bar and the command search of the "zen" interface
        ZenBar {
            anchors.fill: parent
            z: 100

            paintView: notationView.paintView

            position: root.pageModel.railPosition
        }
    }

    statusBar: DockStatusBar {
        objectName: root.pageModel.statusBarName()

        navigationSection: content.navigationSection

        NotationStatusBar {
            id: content
        }
    }

    tours: [
        {
            "eventCode": "online_sounds_added",
            "tour": {
                "id": "online-sounds-first-use",
                "steps": [
                    {
                        "title": qsTrc("playback", "This sound processes online"),
                        "description": qsTrc("playback", "Audio is processed in the background while you work. To trigger processing yourself, turn off automatic processing in Preferences > Audio & MIDI > Online sounds."),
                        "controlUri": "control://NotationStatusBar/NotationStatusBar/OnlineSoundsStatusView",
                        "previewImageOrGifUrl": "qrc:/resources/OnlineSoundsPreview.gif",
                        "videoExplanationUrl": ""
                    }
                ]
            }
        },
        {
            "eventCode": "online_sounds_manual_processing_allowed",
            "tour": {
                "id": "online-sounds-manual-process",
                "steps": [
                    {
                        "title": qsTrc("playback", "Online sounds"),
                        "description": qsTrc("playback", "Click to manually process online sounds."),
                        "controlUri": "control://NotationStatusBar/NotationStatusBar/OnlineSoundsStatusView",
                    }
                ]
            }
        },
    ]
}
