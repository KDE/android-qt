TEMPLATE = lib
VERSION  =
TARGET	 = AndroidSetQtEnv
DESTDIR	 = $$QMAKE_LIBDIR_QT
QT       =

CONFIG	 = shared

SOURCES		= androidsetqtenv.cpp
#INCLUDEPATH	+= tmp $$QMAKE_INCDIR_QT/QtCore $$QMAKE_INCDIR_QT/QtGui $$QT_SOURCE_TREE/src/plugins/graphicssystem/android
#include(../../qbase.pri)
#QMAKE_POST_LINK         =
#QMAKE_LIBS              =
#QMAKE_LFLAGS            =
#QMAKE_LFLAGS_RELEASE    =
#QMAKE_LFLAGS_DEBUG      =
#QMAKE_LFLAGS_APP        =
#QMAKE_LFLAGS_SHLIB      =
#QMAKE_LFLAGS_PLUGIN     =
#QMAKE_LFLAGS_SONAME     =
#QMAKE_LFLAGS_THREAD     =
#QMAKE_RPATH             =
target.path=$$[QT_INSTALL_LIBS]
INSTALLS += target
