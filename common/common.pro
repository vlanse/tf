PATHSTEP = ..
TARGET = common
TEMPLATE = lib
CONFIG += staticlib release

include($${PATHSTEP}/make/make.pri)

SRC_PATH = .

INCLUDEDIR = $${PATHSTEP}/include/$${TARGET}

HEADERS += \
  $${INCLUDEDIR}/error_format.h \
  $${INCLUDEDIR}/error.h \
  $${INCLUDEDIR}/module.h \

SOURCES += \
  $${SRC_PATH}/error_format.cpp \
  $${SRC_PATH}/error.cpp \
