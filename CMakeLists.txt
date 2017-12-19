cmake_minimum_required(VERSION 3.3)
project(total_finder)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
        include
        total-finder
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
        total-finder/create_dir.cpp
        total-finder/create_dir.h
        total-finder/dir_model.cpp
        total-finder/dir_model.h
        total-finder/dir_view_panel.cpp
        total-finder/dir_view_panel.h
        total-finder/edit_file.cpp
        total-finder/edit_file.h
        total-finder/event_filters.cpp
        total-finder/event_filters.h
        total-finder/find_in_files.cpp
        total-finder/find_in_files.h
        total-finder/main.cpp
        total-finder/main_window.cpp
        total-finder/main_window.h
        total-finder/settings.cpp
        total-finder/settings.h
        total-finder/settings_dialog.cpp
        total-finder/settings_dialog.h
        total-finder/shell_utils.cpp
        total-finder/shell_utils.h
        total-finder/tab_context.cpp
        total-finder/tab_context.h
        total-finder/tab_manager.cpp
        total-finder/tab_manager.h
        total-finder/base_panel.cpp
        total-finder/base_panel.h
        total-finder/help_panel.h
        total-finder/help_panel.cpp
)

add_executable(total-finder MACOSX_BUNDLE ${SOURCE_FILES})

target_link_libraries(
        total-finder
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

set_target_properties(
        total-finder PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/bundle.plist.in
)

set_target_properties(total-finder PROPERTIES OUTPUT_NAME "Total Finder")
