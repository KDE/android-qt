load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative script
SOURCES += tst_qdeclarativeqt.cpp
macx:CONFIG -= app_bundle

wince*|symbian: {
    importFiles.files = data
    importFiles.path = .
    DEPLOYMENT += importFiles
    wince*: DEFINES += SRCDIR=\\\".\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

# QMAKE_CXXFLAGS = -fprofile-arcs -ftest-coverage
# LIBS += -lgcov

CONFIG += parallel_test

