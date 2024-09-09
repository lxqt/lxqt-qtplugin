# lxqt-qtplugin

## Overview

This repository provides the library `libqtlxqt` to integrate Qt with LXQt.

With this plugin, all Qt-based programs can adopt settings of LXQt, such as the icon theme and Qt style.

## Installation

### Compiling source code

Runtime dependencies are [libdbusmenu-lxqt](https://github.com/lxqt/libdbusmenu-lxqt), [libfm-qt](https://github.com/lxqt/libfm-qt) and [libqtxdg](https://github.com/lxqt/libqtxdg).
Additional build dependencies are CMake, [lxqt-build-tools](https://github.com/lxqt/lxqt-build-tools), qt6-tools and optionally Git to pull latest VCS checkouts.  

Code configuration is handled by CMake. CMake variable `CMAKE_INSTALL_PREFIX` has to be set to `/usr` on most operating systems.  

To build run `make`, to install `make install` which accepts variable `DESTDIR` as usual.  

### Binary packages

Official binary packages are provided by all major Linux distributions like Arch Linux, Debian, Fedora and openSUSE. Just use your package manager to search for string `lxqt-qtplugin`.

## Configuration, Usage

To use the plugin in Qt, the environment variable `QT_QPA_PLATFORMTHEME=lxqt` has to be exported, such that every Qt application can load the theme plugin.  
If, for some unknown reasons, the plugin is not loaded, we can debug the plugin by exporting `QT_DEBUG_PLUGINS=1`. Qt will print detailed information and error messages about all plugins in the console when running any Qt program.

For Qt5 support please see the [wiki](https://github.com/lxqt/lxqt/wiki/Building-from-source#qt5-support).
