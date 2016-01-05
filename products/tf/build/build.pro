PATHSTEP = ../../..
TEMPLATE = subdirs

SUBDIRS = \
  $${PATHSTEP}/common \
  $${PATHSTEP}/products/tf \

CONFIG += ordered

include($${PATHSTEP}/make/make.pri)
