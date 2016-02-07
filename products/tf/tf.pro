PATHSTEP = ../..
TARGET = tf
DESCRIPTION = Total Finder File Manager
TEMPLATE = app

CONFIG += release
QT += widgets

include($${PATHSTEP}/make/make.pri)

# Application section

LIBS += \
  -lcommon \

POST_TARGETDEPS += \
  $${LIBDIR}/libcommon.a \

FORMS += \
  ui/create_dir.ui \
  ui/dir_view_panel.ui \
  ui/edit_file.ui \
  ui/find_in_files.ui \
  ui/main_window.ui \
  ui/settings_dialog.ui \

HEADERS += \
  create_dir.h \
  dir_model.h \
  dir_view_panel.h \
  edit_file.h \
  event_filters.h \
  find_in_files.h \
  main_window.h \
  settings.h \
  settings_dialog.h \
  svn_supp.h \
  tab_context.h \
  tab_manager.h \

SOURCES += \
  create_dir.cpp \
  dir_model.cpp \
  dir_view_panel.cpp \
  edit_file.cpp \
  event_filters.cpp \
  find_in_files.cpp \
  main.cpp \
  main_window.cpp \
  settings.cpp \
  settings_dialog.cpp \
  svn_supp.cpp \
  tab_context.cpp \
  tab_manager.cpp \
