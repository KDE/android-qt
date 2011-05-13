load(qttest_p4)

SOURCES += ../tst_qtcpsocket.cpp
win32: {
wince*: {
	LIBS += -lws2
} else {
	LIBS += -lws2_32
}
}
QT += network
vxworks:QT -= gui

symbian: {
    TARGET.EPOCHEAPSIZE="0x100 0x3000000"
    TARGET.CAPABILITY = NetworkServices
}

TARGET = tst_qtcpsocket

win32 {
  CONFIG(debug, debug|release) {
    DESTDIR = ../debug
} else {
    DESTDIR = ../release
  }
} else {
    DESTDIR = ../
}


