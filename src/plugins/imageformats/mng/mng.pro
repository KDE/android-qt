TARGET  = qmng
include(../../qpluginbase.pri)

QTDIR_build:REQUIRES = "!contains(QT_CONFIG, no-mng)"

HEADERS += qmnghandler.h
SOURCES += main.cpp \
           qmnghandler.cpp

symbian: {
        #Disable warnings in 3rdparty code due to unused variables and arguments
        QMAKE_CXXFLAGS.CW += -W nounused
        TARGET.UID3=0x2001E619
}

contains(QT_CONFIG, system-mng) {
        unix|win32-g++*:LIBS += -lmng
        win32:!win32-g++*:LIBS += libmng.lib
} else {
        DEFINES += MNG_BUILD_SO
        DEFINES += MNG_NO_INCLUDE_JNG
	INCLUDEPATH += ../../../3rdparty/libmng
	SOURCES  += \
            ../../../3rdparty/libmng/libmng_callback_xs.c \
            ../../../3rdparty/libmng/libmng_chunk_io.c \
            ../../../3rdparty/libmng/libmng_chunk_descr.c \
            ../../../3rdparty/libmng/libmng_chunk_prc.c \
            ../../../3rdparty/libmng/libmng_chunk_xs.c \
            ../../../3rdparty/libmng/libmng_cms.c \
            ../../../3rdparty/libmng/libmng_display.c \
            ../../../3rdparty/libmng/libmng_dither.c \
            ../../../3rdparty/libmng/libmng_error.c \
            ../../../3rdparty/libmng/libmng_filter.c \
            ../../../3rdparty/libmng/libmng_hlapi.c \
            ../../../3rdparty/libmng/libmng_jpeg.c \
            ../../../3rdparty/libmng/libmng_object_prc.c \
            ../../../3rdparty/libmng/libmng_pixels.c \
            ../../../3rdparty/libmng/libmng_prop_xs.c \
            ../../../3rdparty/libmng/libmng_read.c \
            ../../../3rdparty/libmng/libmng_trace.c \
            ../../../3rdparty/libmng/libmng_write.c \
            ../../../3rdparty/libmng/libmng_zlib.c
}

contains(QT_CONFIG, system-zlib) {
    symbian:LIBS_PRIVATE += -llibz
    else:if(unix|win32-g++*):LIBS_PRIVATE += -lz
    else:LIBS += zdll.lib
} else {
    INCLUDEPATH +=  ../../../3rdparty/zlib
}

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target

