include_directories(
  ${QT_INCLUDES}
  ${LXQT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
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
