cmake_minimum_required(VERSION 2.8.6)
project(lxqt-qtplugin)

# Support different versions of Qt
option(USE_QT5 "Build with Qt5." $ENV{USE_QT5})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
if(USE_QT5)
    cmake_minimum_required(VERSION 2.8.11)
    find_package(Qt5Widgets REQUIRED QUIET)
    find_package(Qt5LinguistTools REQUIRED QUIET)

    find_package(lxqt-qt5 REQUIRED QUIET)
else()
    find_package(Qt4 4.6 REQUIRED QtCore QtGui)
    include(${QT_USE_FILE})

    find_package(LXQT REQUIRED)
endif()
include(${LXQT_USE_FILE})

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -Wl,-no-undefined -Wall")
endif()

if(USE_QT5)
    add_subdirectory(qt5)
else()
    add_subdirectory(qt4)
endif()

# building tarball with CPack -------------------------------------------------
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VERSION_MAJOR ${LXQT_MAJOR_VERSION})
set (CPACK_PACKAGE_VERSION_MINOR ${LXQT_MINOR_VERSION})
set (CPACK_PACKAGE_VERSION_PATCH ${LXQT_PATCH_VERSION})
set (CPACK_GENERATOR TBZ2)
set (CPACK_SOURCE_GENERATOR TBZ2)
set (CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include (CPack)
