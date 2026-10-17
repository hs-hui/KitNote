kfw_create_qml_module(kfw_multiwindows_qml ALIAS kfw::multiwindows_qml FOR kfw_multiwindows)

qt_add_qml_module(kfw_multiwindows_qml
    URI KitNote.MultiWindows
    VERSION 1.0
    SOURCES
        multiinstancesdevmodel.cpp
        multiinstancesdevmodel.h
    QML_FILES
        MultiInstancesDevDialog.qml
        MultiInstancesDevPanel.qml
    IMPORTS
        TARGET kfw_ui_qml
        TARGET kfw_uicomponents_qml
)

fixup_qml_module_dependencies(kfw_multiwindows_qml)
