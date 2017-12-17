Total Finder - simple file manager for Mac OSX
===============================

Inspired by widely-recognized Total Commander file manager for Windows.

Built with Qt5 Open Source

## Features

### Implemented
* Multi-tab user interface

### In development
* Search in files and folders (with regexp support etc)
* Support for mounting network shares (cifs, nfs etc)
* Integration with other programs (text editors, diff viewers, VCS)
* Looking inside archives
* Plugins support
* Ability to replace OSX Finder

## How to build
* download and install [Qt5 Open Source](https://www.qt.io)
* download and install [CMake](https://cmake.org)
* set variable ``CMAKE_PREFIX_PATH`` to point to your Qt installation, e.g. 
``CMAKE_PREFIX_PATH=/Users/me/Qt5.9.1/5.9.1/clang_64/lib/cmake``
* run ``cmake . && make``


Development
===========
Some useful tips could be found in [doc](./doc) folder
