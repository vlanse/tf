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
  ui/main_window.ui \
  ui/settings_dialog.ui \

HEADERS += \
  create_dir.h \
  dir_model.h \
  dir_view_panel.h \
  edit_file.h \
  event_filters.h \
  main_window.h \
  settings.h \
  settings_dialog.h \
  tab_manager.h \

SOURCES += \
  create_dir.cpp \
  dir_model.cpp \
  dir_view_panel.cpp \
  edit_file.cpp \
  event_filters.cpp \
  main.cpp \
  main_window.cpp \
  settings.cpp \
  settings_dialog.cpp \
  tab_manager.cpp \
