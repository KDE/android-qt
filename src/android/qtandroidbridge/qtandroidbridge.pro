TEMPLATE = lib
TARGET = QtAndroidBridge
#DESTDIR = $$QMAKE_LIBDIR_QT
QT       =

include(../../qbase.pri)
VERSION=1.0.0

CONFIG+= warn_on
CONFIG-= qt shared create_prl

SOURCES= qtandroidbridge.cpp

HEADERS= qtandroidbridge.h qtandroidbridge_p.h

INCLUDEPATH+= $$QMAKE_INCDIR_QT $$QMAKE_INCDIR_QT/QtCore



