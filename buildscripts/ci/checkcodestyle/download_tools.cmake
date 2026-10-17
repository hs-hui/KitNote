if(NOT DEFINED CMAKE_SCRIPT_MODE_FILE)
    message(FATAL_ERROR "This file is a script")
endif()

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

# KitNote does not download the codestyle tools from MuseScore servers any more.
# The tool files must already be in the checkout, at the paths below.
# Failing here is deliberate: the codestyle check cannot run without them.

function(require_file rel_path)
    if(NOT EXISTS "${REPO_ROOT}/${rel_path}")
        message(FATAL_ERROR "Missing codestyle tool file: ${rel_path}. KitNote no longer downloads it from MuseScore servers; vendor it into the repository first.")
    endif()
endfunction()

# muse_framework: codestyle tools
set(MF_FILES
    format_dir.cmake
    format_file.cmake
    run_scan.sh
    uncrustify.cmake
    uncrustify_muse.cfg
)
foreach(file ${MF_FILES})
    require_file("muse/tools/codestyle/${file}")
endforeach()

# muse_deps: files needed by tools/codestyle/uncrustify.cmake to resolve the uncrustify tool
set(MD_FILES
    buildtools/manifest.cmake
    buildtools/resolve.cmake
    buildtools/build_dependency.cmake
    recipes/uncrustify/meta.cmake
    recipes/uncrustify/spec.cmake
    prebuilt.lock
    prebuilt_url.txt
)
foreach(file ${MD_FILES})
    require_file("muse_deps/${file}")
endforeach()
