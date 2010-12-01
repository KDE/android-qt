load(qttest_p4)
QT += xml svg
contains(QT_CONFIG, opengl)|contains(QT_CONFIG, opengles1)|contains(QT_CONFIG, opengles2):QT += opengl

SOURCES += tst_lancelot.cpp \
           $$QT_SOURCE_TREE/tests/arthur/common/paintcommands.cpp
HEADERS += $$QT_SOURCE_TREE/tests/arthur/common/paintcommands.h
RESOURCES += $$QT_SOURCE_TREE/tests/arthur/common/images.qrc

include($$QT_SOURCE_TREE/tests/arthur/common/baselineprotocol.pri)
win32|symbian*:MKSPEC=$$replace(QMAKESPEC, \\\\, /)
else:MKSPEC=$$QMAKESPEC
DEFINES += QMAKESPEC=\\\"$$MKSPEC\\\"

!symbian:!wince*:DEFINES += SRCDIR=\\\"$$PWD\\\"
