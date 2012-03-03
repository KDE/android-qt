CONFIG += testcase
TARGET = tst_qsqldatabase
SOURCES  += tst_qsqldatabase.cpp

QT += sql testlib

contains(QT_CONFIG, qt3support): QT += qt3support

win32: {
   !wince*: LIBS += -lws2_32
   else: LIBS += -lws2
}

wince*: {
   DEPLOYMENT_PLUGIN += qsqlite

   testData.files = testdata
   testData.path = .

   DEPLOYMENT += testData
}

symbian {
	TARGET.EPOCHEAPSIZE=5000 5000000
	TARGET.EPOCSTACKSIZE=50000

    qt_not_deployed {
        contains(S60_VERSION, 3.1)|contains(S60_VERSION, 3.2)|contains(S60_VERSION, 5.0) {
            sqlite.path = /sys/bin
            sqlite.files = sqlite3.dll
            DEPLOYMENT += sqlite
        }
    }
}

