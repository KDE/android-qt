load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
QT += network
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativepixmapcache.cpp

INCLUDEPATH += ../shared/
HEADERS += ../shared/testhttpserver.h
SOURCES += ../shared/testhttpserver.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"

# QMAKE_CXXFLAGS = -fprofile-arcs -ftest-coverage
# LIBS += -lgcov

CONFIG += parallel_test

