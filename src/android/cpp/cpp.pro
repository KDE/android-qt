TEMPLATE = lib
TARGET = QtAndroidMain
#DESTDIR = $$QMAKE_LIBDIR_QT
QT       =

include(../../qbase.pri)
VERSION=1.0.0

CONFIG+= static staticlib warn_on
CONFIG-= qt shared create_prl

SOURCES= qtmain_android.cpp

INCLUDEPATH+= $$QMAKE_INCDIR_QT $$QMAKE_INCDIR_QT/QtCore


