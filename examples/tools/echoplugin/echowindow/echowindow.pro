HEADERS    = echowindow.h \
             echointerface.h
SOURCES    = echowindow.cpp \
             main.cpp

TARGET     = echoplugin
QMAKE_PROJECT_NAME = echopluginwindow
win32 {
    debug:DESTDIR = ../debug/
    release:DESTDIR = ../release/
} else {
    DESTDIR    = ../
}

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/echoplugin
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS echowindow.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/echoplugin/echowindow
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
