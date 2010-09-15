TEMPLATE = lib
TARGET = QtAndroidBridge
DESTDIR = $$QMAKE_LIBDIR_QT
QT       =

CONFIG+= static staticlib warn_on
CONFIG-= qt shared create_prl

SOURCES= qtandroidbridge.cpp

HEADERS= qtandroidbridge.h

INCLUDEPATH+= $$QMAKE_INCDIR_QT $$QMAKE_INCDIR_QT/QtCore

#include(../qbase.pri)
