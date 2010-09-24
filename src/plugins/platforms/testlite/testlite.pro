TARGET = qtestlite

include(../../qpluginbase.pri)
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES = \
        main.cpp \
        qtestliteintegration.cpp \
        qtestlitewindowsurface.cpp \
        qtestlitewindow.cpp

HEADERS = \
        qtestliteintegration.h \
        qtestlitewindowsurface.h \
        qtestlitewindow.h


LIBS += -lX11 -lXext

include (../fontdatabases/genericunix/genericunix.pri)

contains(QT_CONFIG, opengl) {
    QT += opengl
    HEADERS += qglxintegration.h
    SOURCES += qglxintegration.cpp
}

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
