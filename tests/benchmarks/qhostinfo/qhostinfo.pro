load(qttest_p4)
TEMPLATE = app
TARGET = tst_qhostinfo
DEPENDPATH += .
INCLUDEPATH += .

QT -= gui
QT += network

CONFIG += release

# Input
SOURCES += main.cpp
