cmake_minimum_required(VERSION 3.3)
project(tf)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
        include
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
)

set(SOURCE_FILES
        common/filesystem/osx/copy_file.cpp
        common/filesystem/osx/dir.cpp
        common/filesystem/osx/file_info.cpp
        common/error.cpp
        common/string_utils.cpp
        common/trace.cpp
        include/common/error.h
        include/common/filesystem.h
        include/common/module.h
        include/common/string_utils.h
        include/common/trace.h
        tf/create_dir.cpp
        tf/create_dir.h
        tf/dir_model.cpp
        tf/dir_model.h
        tf/dir_view_panel.cpp
        tf/dir_view_panel.h
        tf/edit_file.cpp
        tf/edit_file.h
        tf/event_filters.cpp
        tf/event_filters.h
        tf/find_in_files.cpp
        tf/find_in_files.h
        tf/main.cpp
        tf/main_window.cpp
        tf/main_window.h
        tf/settings.cpp
        tf/settings.h
        tf/settings_dialog.cpp
        tf/settings_dialog.h
        tf/shell_utils.cpp
        tf/shell_utils.h
        tf/tab_context.cpp
        tf/tab_context.h
        tf/tab_manager.cpp
        tf/tab_manager.h)

add_executable(tf MACOSX_BUNDLE ${SOURCE_FILES})

target_link_libraries(
        tf
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

set_target_properties(tf PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/bundle.plist.in)
