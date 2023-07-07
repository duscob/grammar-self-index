include(GNUInstallDirs)
include(ExternalProject)

set(ExternalProjectName cds)

set(config_flags)  # parameters desired for ./configure of Autotools

set(${ExternalProjectName}_LIBRARY "${CMAKE_STATIC_LIBRARY_PREFIX}${ExternalProjectName}${CMAKE_STATIC_LIBRARY_SUFFIX}")

find_program(MAKE_EXECUTABLE NAMES gmake make mingw32-make REQUIRED)

ExternalProject_Add(
        ${ExternalProjectName}
        GIT_REPOSITORY https://github.com/duscob/libcds.git
        GIT_TAG master
        UPDATE_DISCONNECTED ON
        CONFIGURE_HANDLED_BY_BUILD ON
        CONFIGURE_COMMAND <SOURCE_DIR>/configure ${config_flags}
        BUILD_COMMAND ${MAKE_EXECUTABLE} -j
        #        INSTALL_COMMAND ${MAKE_EXECUTABLE} -j install
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        BUILD_BYPRODUCTS <BINARY_DIR>/src/.libs/${${ExternalProjectName}_LIBRARY}
)

ExternalProject_Get_property(${ExternalProjectName} SOURCE_DIR)
set(${ExternalProjectName}_SOURCE_DIR ${SOURCE_DIR})
include_directories(${${ExternalProjectName}_SOURCE_DIR}/include)

ExternalProject_Get_property(${ExternalProjectName} BINARY_DIR)
set(${ExternalProjectName}_BINARY_DIR ${BINARY_DIR})

add_library(cds::cds INTERFACE IMPORTED GLOBAL)
target_link_libraries(cds::cds INTERFACE "${${ExternalProjectName}_BINARY_DIR}/src/.libs/${${ExternalProjectName}_LIBRARY}")  # need the quotes to expand list
