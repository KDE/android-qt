load(qttest_p4)
TEMPLATE = app
TARGET = tst_bench_qhostinfo
DEPENDPATH += .
INCLUDEPATH += .

QT -= gui
QT += network

CONFIG += release

# Input
SOURCES += main.cpp

symbian: {
    TARGET.CAPABILITY = NetworkServices
    INCLUDEPATH *= $$MW_LAYER_SYSTEMINCLUDE
}
