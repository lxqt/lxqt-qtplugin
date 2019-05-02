cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
# CMP0000: Call the cmake_minimum_required() command at the beginning of the top-level
# CMakeLists.txt file even before calling the project() command.
# The cmake_minimum_required(VERSION) command implicitly invokes the cmake_policy(VERSION)
# command to specify that the current project code is written for the given range of CMake
# versions.
project(lxqt-qtplugin)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Minimum Versions
set(LXQTBT_MINIMUM_VERSION "0.6.0")
set(QT_MINIMUM_VERSION "5.7.1")
set(QTXDG_MINIMUM_VERSION "3.3.0")

find_package(Qt5DBus ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt5LinguistTools ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt5Widgets ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt5XdgIconLoader ${QTXDG_MINIMUM_VERSION} REQUIRED)
find_package(lxqt-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)
find_package(fm-qt)
find_package(dbusmenu-qt5 REQUIRED)

if (fm-qt_FOUND)
    get_target_property(LIB_FM_QT_CONFIGURATIONS fm-qt IMPORTED_CONFIGURATIONS)
    if (LIB_FM_QT_CONFIGURATIONS)
        # Extract the .soname from the first configuration found.
        # We don't use configuration mapping. Any config serves the purpose
        list(GET LIB_FM_QT_CONFIGURATIONS 0 LIB_FM_QT_FIRST_CONFIGURATION)
        get_target_property(LIB_FM_QT_SONAME fm-qt IMPORTED_SONAME_${LIB_FM_QT_FIRST_CONFIGURATION})
    else()
        message(WARNING "libfm-qt found, but no configuration found. Check your libfm-qt installation. Unversioned libfm-qt will be used.")
        set(LIB_FM_QT_SONAME "libfm-qt")
    endif()
else()
    set(LIB_FM_QT_SONAME "libfm-qt")
endif()
mark_as_advanced(LIB_FM_QT_SONAME)

# Patch Version 0

include(LXQtPreventInSourceBuilds)
include(LXQtCompilerSettings NO_POLICY_SCOPE)

add_subdirectory(src)
