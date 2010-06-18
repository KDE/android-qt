# -*-mode:sh-*-
# Qt image handling

# Qt kernel module

HEADERS += \
        image/qbitmap.h \
        image/qicon.h \
        image/qicon_p.h \
        image/qiconloader_p.h \
        image/qiconengine.h \
        image/qiconengineplugin.h \
        image/qimage.h \
        image/qimage_p.h \
        image/qimageiohandler.h \
        image/qimagereader.h \
        image/qimagewriter.h \
        image/qmovie.h \
        image/qnativeimage_p.h \
        image/qpaintengine_pic_p.h \
        image/qpicture.h \
        image/qpicture_p.h \
        image/qpictureformatplugin.h \
        image/qpixmap.h \
        image/qpixmap_raster_p.h \
        image/qpixmap_blitter_p.h \
        image/qpixmapcache.h \
        image/qpixmapcache_p.h \
        image/qpixmapdata_p.h \
        image/qpixmapdatafactory_p.h \
        image/qpixmapfilter_p.h \
        image/qimagepixmapcleanuphooks_p.h \


SOURCES += \
        image/qbitmap.cpp \
        image/qicon.cpp \
        image/qiconloader.cpp \
        image/qimage.cpp \
        image/qimageiohandler.cpp \
        image/qimagereader.cpp \
        image/qimagewriter.cpp \
        image/qpaintengine_pic.cpp \
        image/qpicture.cpp \
        image/qpictureformatplugin.cpp \
        image/qpixmap.cpp \
        image/qpixmapcache.cpp \
        image/qpixmapdata.cpp \
        image/qpixmapdatafactory.cpp \
        image/qpixmapfilter.cpp \
        image/qiconengine.cpp \
        image/qiconengineplugin.cpp \
        image/qmovie.cpp \
        image/qpixmap_raster.cpp \
        image/qpixmap_blitter.cpp \
        image/qnativeimage.cpp \
        image/qimagepixmapcleanuphooks.cpp \


win32 {
    SOURCES += image/qpixmap_win.cpp
}
embedded {
    SOURCES += image/qpixmap_qws.cpp
}
embedded_lite {
    SOURCES += image/qpixmap_lite.cpp
}
x11 {
    HEADERS += image/qpixmap_x11_p.h
    SOURCES += image/qpixmap_x11.cpp 
}
!embedded:!embedded_lite:mac {
    HEADERS += image/qpixmap_mac_p.h
    SOURCES += image/qpixmap_mac.cpp
}
symbian {
    HEADERS += image/qpixmap_s60_p.h
    SOURCES += image/qpixmap_s60.cpp
}

# Built-in image format support
HEADERS += \
        image/qbmphandler_p.h \
        image/qppmhandler_p.h \
        image/qxbmhandler_p.h \
        image/qxpmhandler_p.h

SOURCES += \
        image/qbmphandler.cpp \
        image/qppmhandler.cpp \
        image/qxbmhandler.cpp \
        image/qxpmhandler.cpp

# 3rd party / system PNG support
!contains(QT_CONFIG, no-png) {
    HEADERS += image/qpnghandler_p.h
    SOURCES += image/qpnghandler.cpp

    contains(QT_CONFIG, system-png) {
        unix|win32-g++*:LIBS_PRIVATE  += -lpng
        win32:!win32-g++*:LIBS += libpng.lib
    } else {
	DEFINES *= QT_USE_BUNDLED_LIBPNG
        !isEqual(QT_ARCH, i386):!isEqual(QT_ARCH, x86_64):DEFINES += PNG_NO_ASSEMBLER_CODE
        INCLUDEPATH += ../3rdparty/libpng
        SOURCES += ../3rdparty/libpng/png.c \
          ../3rdparty/libpng/pngerror.c \
          ../3rdparty/libpng/pngget.c \
          ../3rdparty/libpng/pngmem.c \
          ../3rdparty/libpng/pngpread.c \
          ../3rdparty/libpng/pngread.c \
          ../3rdparty/libpng/pngrio.c \
          ../3rdparty/libpng/pngrtran.c \
          ../3rdparty/libpng/pngrutil.c \
          ../3rdparty/libpng/pngset.c \
          ../3rdparty/libpng/pngtrans.c \
          ../3rdparty/libpng/pngwio.c \
          ../3rdparty/libpng/pngwrite.c \
          ../3rdparty/libpng/pngwtran.c \
          ../3rdparty/libpng/pngwutil.c

        contains(QT_CONFIG, system-zlib) {
            symbian:LIBS_PRIVATE += -llibz
            else:if(unix|win32-g++*):LIBS_PRIVATE += -lz
            else:LIBS += zdll.lib
        } else {
            INCLUDEPATH  += ../3rdparty/zlib
        }
    }
} else {
    DEFINES *= QT_NO_IMAGEFORMAT_PNG
}
