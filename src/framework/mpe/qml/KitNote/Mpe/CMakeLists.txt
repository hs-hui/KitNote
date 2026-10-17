kfw_create_qml_module(kfw_mpe_qml ALIAS kfw::mpe_qml FOR kfw_mpe)

qt_add_qml_module(kfw_mpe_qml
    URI KitNote.Mpe
    VERSION 1.0
    SOURCES
        articulationpatternsegmentitem.cpp
        articulationpatternsegmentitem.h
        articulationpatternitem.cpp
        articulationpatternitem.h
        articulationsprofileeditormodel.cpp
        articulationsprofileeditormodel.h
    QML_FILES
        ArrangementPatternControl.qml
        ArticulationPatternControlsTab.qml
        ArticulationPatternPlot.qml
        ArticulationPatternSegmentsList.qml
        ArticulationPatternsList.qml
        ArticulationPatternsTab.qml
        ExpressionPatternControl.qml
        PitchPatternControl.qml
    IMPORTS
        TARGET kfw_ui_qml
        TARGET kfw_uicomponents_qml
)

fixup_qml_module_dependencies(kfw_mpe_qml)
