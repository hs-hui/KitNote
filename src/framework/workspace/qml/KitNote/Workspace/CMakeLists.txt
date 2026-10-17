kfw_create_qml_module(kfw_workspace_qml ALIAS kfw::workspace_qml FOR kfw_workspace)

qt_add_qml_module(kfw_workspace_qml
    URI KitNote.Workspace
    VERSION 1.0
    SOURCES
        newworkspacemodel.cpp
        newworkspacemodel.h
        workspacelistmodel.cpp
        workspacelistmodel.h
        workspacesmenumodel.cpp
        workspacesmenumodel.h
    QML_FILES
        NewWorkspaceDialog.qml
        WorkspacesDialog.qml
        internal/WorkspacesTopPanel.qml
        internal/WorkspacesView.qml
    IMPORTS
        TARGET kfw_ui_qml
        TARGET kfw_uicomponents_qml
)

fixup_qml_module_dependencies(kfw_workspace_qml)
