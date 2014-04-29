# version >= 2.8.6 is needed because of CMAKE_AUTOMOC feature
cmake_minimum_required(VERSION 2.8.6)
project(lxqt-qtplugin)

# Standard directories for installation
# include(GNUInstallDirs)

# FIXME: support Qt5
find_package(Qt4 4.6 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})

find_package(LxQt REQUIRED)
include(${LXQT_USE_FILE})

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -Wl,-no-undefined -Wall")
endif()

# Eanble CMake auto-moc support for Qt
set(CMAKE_AUTOMOC TRUE)

include_directories(
  ${QT_INCLUDES}
  ${LXQT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(qtlxqt_SRCS
  lxqtguiplatformplugin.cpp
  lxqtguiplatformplugin.h
  qguiplatformplugin_p.h
)

add_library(qtlxqt SHARED ${qtlxqt_SRCS})

target_link_libraries(qtlxqt
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${LXQT_LIBRARIES}
)

install (TARGETS qtlxqt LIBRARY DESTINATION ${QT_PLUGINS_DIR}/gui_platform)


# building tarball with CPack -------------------------------------------------
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VERSION_MAJOR ${LXQT_MAJOR_VERSION})
set (CPACK_PACKAGE_VERSION_MINOR ${LXQT_MINOR_VERSION})
set (CPACK_PACKAGE_VERSION_PATCH ${LXQT_PATCH_VERSION})
set (CPACK_GENERATOR TBZ2)
set (CPACK_SOURCE_GENERATOR TBZ2)
set (CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include (CPack)
