include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    "${CMAKE_CURRENT_BINARY_DIR}"
)

set(qtlxqt_HDRS
    lxqtplatformtheme.h
)

set(qtlxqt_SRCS
    main.cpp
    lxqtplatformtheme.cpp
)

add_library(qtlxqt MODULE ${qtlxqt_HDRS} ${qtlxqt_SRCS})

target_compile_definitions(qtlxqt
    PRIVATE
        "QT_NO_FOREACH"
        "LIB_FM_QT_SONAME=\"${LIB_FM_QT_SONAME}\""
)

target_link_libraries(qtlxqt
    Qt5::Widgets
    Qt5::ThemeSupportPrivate
    Qt5XdgIconLoader
)


# there is no standard way to get the plugin dir of Qt5 with cmake
# The best way is get_target_property(QT_PLUGINS_DIR Qt5::QGtk2ThemePlugin LOCATION).
# This directly returns the platformthemes dir. However, this does not work
# in some distros, such as ubuntu.
# Finally, I came up with a more reliable way by using qmake.
get_target_property(QT_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE} IMPORTED_LOCATION)
if(NOT QT_QMAKE_EXECUTABLE)
    message(FATAL_ERROR "qmake is not found.")
endif()

# execute the command "qmake -query QT_INSTALL_PLUGINS" to get the path of plugins dir.
execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
    OUTPUT_VARIABLE QT_PLUGINS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(QT_PLUGINS_DIR)
    message(STATUS "Qt5 plugin directory:" "${QT_PLUGINS_DIR}")
else()
    message(FATAL_ERROR "Qt5 plugin directory cannot be detected.")
endif()

install(TARGETS qtlxqt LIBRARY DESTINATION "${QT_PLUGINS_DIR}/platformthemes")
