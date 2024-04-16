include_directories(
    ${Qt6Gui_PRIVATE_INCLUDE_DIRS}
    "${CMAKE_CURRENT_BINARY_DIR}"
)

set(qtlxqt_HDRS
    lxqtplatformtheme.h
    lxqtsystemtrayicon.h
    statusnotifieritem/statusnotifieritem.h
    statusnotifieritem/dbustypes.h
)

set(qtlxqt_SRCS
    main.cpp
    lxqtplatformtheme.cpp
    lxqtsystemtrayicon.cpp
    statusnotifieritem/statusnotifieritem.cpp
    statusnotifieritem/dbustypes.cpp
)

qt6_add_dbus_adaptor(qtlxqt_SRCS
    statusnotifieritem/org.kde.StatusNotifierItem.xml
    statusnotifieritem/statusnotifieritem.h
    StatusNotifierItem
)

add_library(qtlxqt MODULE ${qtlxqt_HDRS} ${qtlxqt_SRCS})

target_compile_definitions(qtlxqt
    PRIVATE
        "QT_NO_FOREACH"
        "LIB_FM_QT_SONAME=\"${LIB_FM_QT_SONAME}\""
)

target_link_libraries(qtlxqt
    Qt6::Widgets
    Qt6::DBus
    dbusmenu-lxqt
    Qt6XdgIconLoader
)

lxqt_query_qt(_QT_PLUGINS_DIR QT_INSTALL_PLUGINS)

if (NOT _QT_PLUGINS_DIR)
    message(FATAL_ERROR "Qt6 plugin directory cannot be detected.")
endif()
set(QT_PLUGINS_DIR "${_QT_PLUGINS_DIR}")
message(STATUS "Qt6 plugin directory:" "${QT_PLUGINS_DIR}")

install(TARGETS qtlxqt LIBRARY DESTINATION "${QT_PLUGINS_DIR}/platformthemes")
