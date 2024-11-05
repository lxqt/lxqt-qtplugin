cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
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
set(LXQTBT_MINIMUM_VERSION "2.1.0")
set(QT_MINIMUM_VERSION "6.6.0")
set(QTXDG_MINIMUM_VERSION "4.1.0")
set(DBUSMENU_MINIMUM_VERSION "0.2.0")
set(FMQT_MINIMUM_VERSION "2.1.0")

find_package(Qt6DBus ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt6LinguistTools ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt6Widgets ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt6XdgIconLoader ${QTXDG_MINIMUM_VERSION} REQUIRED)
find_package(lxqt2-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)
find_package(dbusmenu-lxqt ${DBUSMENU_MINIMUM_VERSION} REQUIRED)
find_package(fm-qt6 ${FMQT_MINIMUM_VERSION} REQUIRED)

get_target_property(LIB_FM_QT_CONFIGURATIONS fm-qt6 IMPORTED_CONFIGURATIONS)
if (LIB_FM_QT_CONFIGURATIONS)
    # Extract the .soname from the first configuration found.
    # We don't use configuration mapping. Any config serves the purpose
    list(GET LIB_FM_QT_CONFIGURATIONS 0 LIB_FM_QT_FIRST_CONFIGURATION)
    get_target_property(LIB_FM_QT_SONAME fm-qt6 IMPORTED_SONAME_${LIB_FM_QT_FIRST_CONFIGURATION})
else()
    message(ERROR "libfm-qt, but no configuration found. Check your libfm-qt installation.")
endif()
mark_as_advanced(LIB_FM_QT_SONAME)

include(LXQtPreventInSourceBuilds)
include(LXQtCompilerSettings NO_POLICY_SCOPE)
include(LXQtQueryQt)

add_subdirectory(src)
