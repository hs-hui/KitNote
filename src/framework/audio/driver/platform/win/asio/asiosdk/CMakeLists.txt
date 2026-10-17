
kfw_create_thirdparty_module(asiosdk)

if(KFW_MODULE_AUDIO_ASIO_SDK_PATH)
    set(ASIO_SDK_PATH ${KFW_MODULE_AUDIO_ASIO_SDK_PATH})
else()
    # Vendored: thirdparty/asiosdk holds the files the build needs from the
    # Steinberg ASIO SDK 2.3.4 (GPLv3 variant, see thirdparty/asiosdk/ASIOSDK/LICENSE.txt).
    # Nothing is downloaded.
    set(ASIO_SDK_PATH ${PROJECT_SOURCE_DIR}/thirdparty/asiosdk)
endif()

target_sources(asiosdk PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/asiosdk_asio.cpp
    ${CMAKE_CURRENT_LIST_DIR}/asiosdk_asiolist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/asiosdk_asiodrivers.cpp
)

target_include_directories(asiosdk PUBLIC
    ${ASIO_SDK_PATH}
    ${ASIO_SDK_PATH}/ASIOSDK/common
    ${ASIO_SDK_PATH}/ASIOSDK/host
    ${ASIO_SDK_PATH}/ASIOSDK/host/pc
)

# Silence warnings from SDK
target_compile_options(asiosdk PRIVATE /wd4267)
