load(qttest_p4)
SOURCES  += tst_qfileinfo.cpp

QT = core


RESOURCES      += qfileinfo.qrc

wince*:|symbian: {
    deploy.files += qfileinfo.qrc tst_qfileinfo.cpp
    res.files = resources\\file1 resources\\file1.ext1 resources\\file1.ext1.ext2
    res.path = resources
    DEPLOYMENT += deploy res
}

win32*:LIBS += -ladvapi32 -lnetapi32

symbian {
    TARGET.CAPABILITY=AllFiles
    LIBS *= -lefsrv
    INCLUDEPATH *= $$MW_LAYER_SYSTEMINCLUDE  # Needed for e32svr.h in S^3 envs
    }

# support for running test from shadow build directory
wince* {
    DEFINES += SRCDIR=\\\"\\\"
} else:symbian {
    # do not define SRCDIR at all
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

contains(QT_CONFIG, qt3support): QT += qt3support

CONFIG += parallel_test
