load(qttest_p4)
SOURCES += tst_qfilesystemwatcher.cpp
QT = core

CONFIG += parallel_test
mac*:CONFIG+=insignificant_test # QTQAINFRA-428
