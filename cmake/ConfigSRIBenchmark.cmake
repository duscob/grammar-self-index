# Adapted from https://github.com/Crascit/DownloadProject/blob/master/CMakeLists.txt
#
# CAVEAT: use DownloadProject.cmake
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else ()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif ()

set(ExternalProjectName sr-index)

include(DownloadProject)
download_project(PROJ ${ExternalProjectName}
        GIT_REPOSITORY https://github.com/duscob/sr-index.git
        GIT_TAG main
        ${UPDATE_DISCONNECTED_IF_AVAILABLE})

include_directories("${${ExternalProjectName}_SOURCE_DIR}/benchmark")
