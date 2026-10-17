kfw_create_qml_module(kfw_legacytreeview_qml ALIAS kfw::legacytreeview_qml FOR kfw_uicomponents)

qt_add_qml_module(kfw_legacytreeview_qml
    URI KitNote.UiComponents.LegacyTreeView
    VERSION 1.0
    SOURCES
        qquicktreemodeladaptor.cpp
        qquicktreemodeladaptor_p.h
        qquickabstractstyle.cpp
        qquickabstractstyle_p.h
        qquickpadding_p.h
        qquickselectionmode_p.h
    QML_FILES
        BasicTableView.qml
        BasicTableViewStyle.qml
        LegacyTreeView.qml
        LegacyTreeViewItemDelegateLoader.qml
        LegacyTreeViewStyle.qml
        Style.qml
        TableViewColumn.qml
        TableViewItemDelegateLoader.qml
    RESOURCES
        images/header.png
    IMPORTS
        TARGET kfw_uicomponents_qml
)

fixup_qml_module_dependencies(kfw_legacytreeview_qml)