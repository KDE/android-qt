load(qttest_p4)

SOURCES += tst_qaudioinput.cpp

QT = core multimedia

wince* {
    deploy.files += 4.wav
    DEPLOYMENT += deploy
    DEFINES += SRCDIR=\\\"\\\"
    QT += gui
} else {
    !symbian:DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

