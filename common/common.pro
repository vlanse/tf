PATHSTEP = ..
TARGET = common
TEMPLATE = lib
CONFIG += staticlib release

include($${PATHSTEP}/make/make.pri)

SRC_PATH = .

INCLUDEDIR = $${PATHSTEP}/include/$${TARGET}

HEADERS += \
  $${INCLUDEDIR}/error.h \
  $${INCLUDEDIR}/module.h \
  $${INCLUDEDIR}/string_utils.h 
  $${INCLUDEDIR}/trace.h \

SOURCES += \
  $${SRC_PATH}/error.cpp \
  $${SRC_PATH}/string_utils.cpp \
  $${SRC_PATH}/trace.cpp \
