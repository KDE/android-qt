load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
QT += sql script
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativesqldatabase.cpp

wince*|symbian: {
    importFiles.files = data
    importFiles.path = .
    DEPLOYMENT += importFiles
    wince*: DEFINES += SRCDIR=\\\".\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test

