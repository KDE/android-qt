load(qttest_p4)
SOURCES += tst_xmlpatterns.cpp \
           ../qxmlquery/TestFundament.cpp

wince* {
DEFINES += SRCDIR=\\\"./\\\"
} else:!symbian {
DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

include (../xmlpatterns.pri)

CONFIG+=insignificant_test # QTQAINFRA-428
