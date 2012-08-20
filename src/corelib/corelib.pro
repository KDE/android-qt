TARGET	   = QtCore
QPRO_PWD   = $$PWD
QT         =
DEFINES   += QT_BUILD_CORE_LIB QT_NO_USING_NAMESPACE
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x67000000
irix-cc*:QMAKE_CXXFLAGS += -no_prelink -ptused

include(../qbase.pri)
include(animation/animation.pri)
include(arch/arch.pri)
include(concurrent/concurrent.pri)
include(global/global.pri)
include(thread/thread.pri)
include(tools/tools.pri)
include(io/io.pri)
include(plugin/plugin.pri)
include(kernel/kernel.pri)
include(codecs/codecs.pri)
include(statemachine/statemachine.pri)
include(xml/xml.pri)

!qpa:mac|darwin:LIBS_PRIVATE += -framework ApplicationServices
qpa {
    contains(QT_CONFIG, coreservices) {
        LIBS_PRIVATE += -framework CoreServices -framework Security
    }
} else:mac|darwin {
        LIBS_PRIVATE += -framework CoreFoundation -framework Security
}
mac:lib_bundle:DEFINES += QT_NO_DEBUG_PLUGIN_CHECK
win32:DEFINES-=QT_NO_CAST_TO_ASCII

QMAKE_LIBS += $$QMAKE_LIBS_CORE

QMAKE_DYNAMIC_LIST_FILE = $$PWD/QtCore.dynlist

contains(DEFINES,QT_EVAL):include(eval.pri)

symbian: {
    TARGET.UID3=0x2001B2DC

    # Problems using data exports from this DLL mean that we can't page it on releases that don't support
    # data exports (currently that's any release before Symbian^3)
    pagingBlock = "$${LITERAL_HASH}ifndef SYMBIAN_DLL_DATA_EXPORTS_SUPPORTED" \
                  "UNPAGED" \
                  "$${LITERAL_HASH}endif"
    MMP_RULES += pagingBlock
    LIBS += -ltzclient
}

android: {
    CURRENT_PWD=$$PWD
    message($$CURRENT_PWD $$OBJECTS_DIR $$QMAKE_AR_ANDROID)
    ret = $$system(mkdir -p $$OBJECTS_DIR/stl && cd $$OBJECTS_DIR/stl && $$QMAKE_AR_ANDROID x $$ANDROID_SOURCES_CXX_STL_LIBDIR/libgnustl_static.a)
    STL_FILES = $$system(ls $$OBJECTS_DIR/stl)
    message($$join(STL_FILES, $$LITERAL_HASH))
    for(file, STL_FILES) {
        exists($$OBJECTS_DIR/stl/$$file) {
            OBJECTS += $$OBJECTS_DIR/stl/$$file
            message("Add stl object file $$OBJECTS_DIR/stl/$$file")
        }else : message("Skip stl object file $$OBJECTS_DIR/stl/$$file")
    }
    ret = $$system($$CURRENT_PWD)
    QMAKE_LFLAGS += -fPIC  -Wl,--verbose
    #add whole gnu_stl library to QtCore
#     QMAKE_LFLAGS += -Wl,--export-dynamic -fPIC  -Wl,--verbose -Wl,--whole-archive $$ANDROID_SOURCES_CXX_STL_LIBDIR/libgnustl_static.a -Wl,--no-whole-archive
#     LIBS_PRIVATE +=
#     QMAKE_LIBS   -=  -lz -lm -ldl -lc -lgcc
    # QMAKE_LFLAGS += -Wl,--whole-archive -Wl,--export-dynamic $$ANDROID_SOURCES_CXX_STL_LIBDIR/libgnustl_static.a -Wl,--no-whole-archive
}
