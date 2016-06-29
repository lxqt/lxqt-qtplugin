cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project(lxqt-qtplugin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED QUIET)
find_package(Qt5LinguistTools REQUIRED QUIET)

#Build with our StatusNotifierItem implementation?
#if not defined by user, decide by Qt version
if (NOT DEFINED WITH_SNI)
    if (Qt5Widgets_VERSION VERSION_LESS 5.6.1)
        set(WITH_SNI ON)
    else ()
        set(WITH_SNI OFF)
    endif ()
endif ()
if (WITH_SNI)
    message(STATUS "Building with our StatusNotifierItem implementation")
    find_package(Qt5DBus REQUIRED QUIET)
    find_package(dbusmenu-qt5 REQUIRED QUIET)
else ()
    message(STATUS "Building w/o our StatusNotifierItem implementation")
endif ()

find_package(lxqt REQUIRED QUIET)

include(LXQtCompilerSettings NO_POLICY_SCOPE)

add_subdirectory(src)

# building tarball with CPack -------------------------------------------------
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR ${LXQT_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${LXQT_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${LXQT_PATCH_VERSION})
set(CPACK_GENERATOR TBZ2)
set(CPACK_SOURCE_GENERATOR TBZ2)
set(CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include(CPack)
