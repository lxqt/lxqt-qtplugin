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
# The best way is get_target_property(QT_PLUGINS_DIR Qt5::QGtk2ThemePlugin LOCATION).
# This directly returns the platformthemes dir.
# However, I'm not sure if Qt5::QGtk2ThemePlugin exists in every distro
# Let's use a stupid but safer method, which is well documented in Qt doc.
foreach(plugin ${Qt5Gui_PLUGINS}) # iterate over all plugins
	get_target_property(plugin_dir ${plugin} LOCATION)
	if(plugin_dir)
		# remove the filename part
		get_filename_component(plugin_dir ${plugin_dir} PATH)
		# go to parent dir (should be something like /usr/lib/qt/plugins)
		get_filename_component(QT_PLUGINS_DIR ${plugin_dir} PATH)
		break()
	endif()
endforeach()
message(STATUS "Qt5 plugin directory:" ${QT_PLUGINS_DIR})

install (TARGETS qtlxqt LIBRARY DESTINATION ${QT_PLUGINS_DIR}/platformthemes)
