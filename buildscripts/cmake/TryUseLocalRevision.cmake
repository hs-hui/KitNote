
if (KITNOTE_REVISION STREQUAL "")
    if(EXISTS "${PROJECT_SOURCE_DIR}/local_build_revision.env")
        file(READ "${PROJECT_SOURCE_DIR}/local_build_revision.env" KITNOTE_REVISION)
        STRING(REGEX REPLACE "\n" "" KITNOTE_REVISION "${KITNOTE_REVISION}")
    endif()
endif()
