message(STATUS "Configuring kfw_pch")

add_library(kfw_pch pch.cpp)

target_precompile_headers_clang_ccache(kfw_pch PRIVATE pch.h)

target_link_libraries(kfw_pch PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Quick
)
