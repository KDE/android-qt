load(qttest_p4)
SOURCES  += tst_qchar.cpp

QT = core

wince*|symbian*: {
deploy.sources += NormalizationTest.txt
DEPLOYMENT = deploy
}

DEFINES += SRCDIR=\\\"$$PWD/\\\"
