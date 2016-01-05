BINDIR = void
BUILDMODE = void

# use this pathstep to reach original build directory on 'make' stage (+1 level for shadow dir).
MAKE_PATHSTEP = $${PATHSTEP}/..

CONFIG(release, release|debug){
  BUILDMODE = release
}

CONFIG(debug, release|debug){
  BUILDMODE = debug
}

CONFIG += c++11

SHADOW_DIR = .shadow

contains(TEMPLATE, lib){
  BINDIR = $${SHADOW_DIR}/lib
  CONFIG(staticlib, staticlib|dll){
    !build_pass:message( "Configuring '$${TARGET}' static library $${BUILDMODE} build..." )
  }
  CONFIG(dll, staticlib|dll){
    TARGET_EXT = .dll
    !build_pass:message( "Configuring '$${TARGET}' dynamic library $${BUILDMODE} build..." )
    DLLDESTDIR = $${PATHSTEP}/bin/$${BUILDMODE}/$${BUILDENV}
  }
  plugin{
    !build_pass:message( "Configuring '$${TARGET}' plugin $${BUILDMODE} build..." )
    DLLDESTDIR = $${PATHSTEP}/bin/$${BUILDMODE}/$${BUILDENV}
  }
}

contains(TEMPLATE, app){
  BINDIR = $${SHADOW_DIR}/bin
  !build_pass:message( "Configuring '$${TARGET}' executable $${BUILDMODE} build..." )
}

contains(TEMPLATE, subdirs){
  BINDIR = $${SHADOW_DIR}/bin
  !build_pass:message( "Configuring subdirs build..." )
}

OBJECTS_DIR = $${PATHSTEP}/$${SHADOW_DIR}/obj/$${TARGET}/$${BUILDMODE}

DESTDIR = $${PATHSTEP}/$${BINDIR}/$${BUILDMODE}
MOC_DIR = $${OBJECTS_DIR}
RCC_DIR = $${OBJECTS_DIR}
UI_DIR = $${PATHSTEP}/$${SHADOW_DIR}/obj/$${TARGET}/ui

LIBS += -L$${PATHSTEP}/$${SHADOW_DIR}/lib/$${BUILDMODE}

LIBDIR = $${PATHSTEP}/$${SHADOW_DIR}/lib/$${BUILDMODE}

INCLUDEPATH += \
  $${PATHSTEP} \
  $${PATHSTEP}/include \
  $${PATHSTEP}/text \
  $${PATHSTEP}/make \
  $${UI_DIR} \
