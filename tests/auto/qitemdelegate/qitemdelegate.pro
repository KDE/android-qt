CONFIG += testcase

TARGET = tst_qitemdelegate
QT += testlib
SOURCES += tst_qitemdelegate.cpp

win32:!wince*: LIBS += -lUser32

CONFIG+=insignificant_test # QTQAINFRA-428
