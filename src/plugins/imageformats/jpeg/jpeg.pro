TARGET  = qjpeg
include(../../qpluginbase.pri)

QTDIR_build:REQUIRES = "!contains(QT_CONFIG, no-jpeg)"

HEADERS += qjpeghandler.h
SOURCES += main.cpp \
           qjpeghandler.cpp

wince*: { 
	DEFINES += NO_GETENV 
	contains(CE_ARCH,x86):CONFIG -= stl exceptions
	contains(CE_ARCH,x86):CONFIG += exceptions_off
}

#Disable warnings in 3rdparty code due to unused arguments
symbian: {
        QMAKE_CXXFLAGS.CW += -W nounusedarg
        TARGET.UID3=0x2001E61B
} else:contains(QMAKE_CC, gcc): {
	QMAKE_CFLAGS_WARN_ON += -Wno-unused-parameter -Wno-main
}

contains(QT_CONFIG, system-jpeg) {
        unix:LIBS += -ljpeg
        win32:LIBS += libjpeg.lib
}
!contains(QT_CONFIG, system-jpeg) {
	INCLUDEPATH += ../../../3rdparty/libjpeg
	SOURCES  += \
	    ../../../3rdparty/libjpeg/jaricom.c \
	    ../../../3rdparty/libjpeg/jcapimin.c \
	    ../../../3rdparty/libjpeg/jcapistd.c \
	    ../../../3rdparty/libjpeg/jcarith.c \
	    ../../../3rdparty/libjpeg/jccoefct.c \
	    ../../../3rdparty/libjpeg/jccolor.c \
	    ../../../3rdparty/libjpeg/jcdctmgr.c \
	    ../../../3rdparty/libjpeg/jchuff.c \
	    ../../../3rdparty/libjpeg/jcinit.c \
	    ../../../3rdparty/libjpeg/jcmainct.c \
	    ../../../3rdparty/libjpeg/jcmarker.c \
	    ../../../3rdparty/libjpeg/jcmaster.c \
	    ../../../3rdparty/libjpeg/jcomapi.c \
	    ../../../3rdparty/libjpeg/jcparam.c \
	    ../../../3rdparty/libjpeg/jcprepct.c \
	    ../../../3rdparty/libjpeg/jcsample.c \
	    ../../../3rdparty/libjpeg/jctrans.c \
	    ../../../3rdparty/libjpeg/jdapimin.c \
	    ../../../3rdparty/libjpeg/jdapistd.c \
	    ../../../3rdparty/libjpeg/jdarith.c \
	    ../../../3rdparty/libjpeg/jdatadst.c \
	    ../../../3rdparty/libjpeg/jdatasrc.c \
	    ../../../3rdparty/libjpeg/jdcoefct.c \
	    ../../../3rdparty/libjpeg/jdcolor.c \
	    ../../../3rdparty/libjpeg/jddctmgr.c \
	    ../../../3rdparty/libjpeg/jdhuff.c \
	    ../../../3rdparty/libjpeg/jdinput.c \
	    ../../../3rdparty/libjpeg/jdmainct.c \
	    ../../../3rdparty/libjpeg/jdmarker.c \
	    ../../../3rdparty/libjpeg/jdmaster.c \
	    ../../../3rdparty/libjpeg/jdmerge.c \
	    ../../../3rdparty/libjpeg/jdpostct.c \
	    ../../../3rdparty/libjpeg/jdsample.c \
	    ../../../3rdparty/libjpeg/jdtrans.c \
	    ../../../3rdparty/libjpeg/jerror.c \
	    ../../../3rdparty/libjpeg/jfdctflt.c \
	    ../../../3rdparty/libjpeg/jfdctfst.c \
	    ../../../3rdparty/libjpeg/jfdctint.c \
	    ../../../3rdparty/libjpeg/jidctflt.c \
	    ../../../3rdparty/libjpeg/jidctfst.c \
	    ../../../3rdparty/libjpeg/jidctint.c \
	    ../../../3rdparty/libjpeg/jquant1.c \
	    ../../../3rdparty/libjpeg/jquant2.c \
	    ../../../3rdparty/libjpeg/jutils.c \
	    ../../../3rdparty/libjpeg/jmemmgr.c \
	    ../../../3rdparty/libjpeg/jmemnobs.c
}

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target

