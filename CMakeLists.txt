cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project(lxqt-qtplugin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED QUIET)
find_package(Qt5LinguistTools REQUIRED QUIET)
find_package(Qt5DBus REQUIRED QUIET)
find_package(dbusmenu-qt5 REQUIRED QUIET)

find_package(lxqt REQUIRED QUIET)
find_package(Qt5XdgIconLoader REQUIRED QUIET)

include(LXQtCompilerSettings NO_POLICY_SCOPE)

add_subdirectory(src)
