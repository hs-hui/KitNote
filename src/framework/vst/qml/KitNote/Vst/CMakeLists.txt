kfw_create_qml_module(kfw_vst_qml ALIAS kfw::vst_qml FOR kfw_vst)

qt_add_qml_module(kfw_vst_qml
    URI KitNote.Vst
    VERSION 1.0
    SOURCES
        vstview.cpp
        vstview.h
    QML_FILES
        VstEditor.qml
        VstEditorDialog.qml
    IMPORTS
        TARGET kfw_ui_qml
        TARGET kfw_uicomponents_qml
)

fixup_qml_module_dependencies(kfw_vst_qml)

if (OS_IS_LIN OR OS_IS_FBSD)
    target_sources(kfw_vst_qml PRIVATE
        platform/linux/runloop.cpp
        platform/linux/runloop.h
    )
endif()

target_link_libraries(kfw_vst_qml PRIVATE vst3sdk)
