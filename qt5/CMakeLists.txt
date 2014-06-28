include_directories(
  ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(qtlxqt_SRCS
  main.cpp
  lxqtplatformtheme.cpp
)

add_library(qtlxqt SHARED ${qtlxqt_SRCS})

target_link_libraries(qtlxqt
  Qt5::Widgets
)

# there is no standard way to get the plugin dir of Qt5 with cmake
get_target_property(QT_PLUGINS_DIR Qt5::Gui LOCATION)
get_filename_component(QT_PLUGINS_DIR ${QT_PLUGINS_DIR} PATH)
set(QT_PLUGINS_DIR ${QT_PLUGINS_DIR}/qt5/plugins)
message(STATUS "Qt5 plugin directory:" ${QT_PLUGINS_DIR})

install (TARGETS qtlxqt LIBRARY DESTINATION ${QT_PLUGINS_DIR}/platformthemes)
