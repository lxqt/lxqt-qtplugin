cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project(lxqt-qtplugin)

include(GNUInstallDirs)

set(LXQTBT_MINIMUM_VERSION "0.5.0")
set(LIBFM_QT_MINIMUM_VERSION "0.14.0")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(dbusmenu-qt5 REQUIRED)
find_package(lxqt-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)
find_package(Qt5XdgIconLoader REQUIRED)

# Patch Version 0

# for file dialog support
find_package(Qt5X11Extras REQUIRED)
find_package(fm-qt ${LIBFM_QT_MINIMUM_VERSION} REQUIRED)

include(LXQtPreventInSourceBuilds)
include(LXQtCompilerSettings NO_POLICY_SCOPE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_subdirectory(src)
