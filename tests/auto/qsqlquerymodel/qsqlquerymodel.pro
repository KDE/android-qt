load(qttest_p4)
SOURCES  += tst_qsqlquerymodel.cpp

QT += sql

wince*: {
   DEPLOYMENT_PLUGIN += qsqlite
	LIBS += -lws2
}else:symbian {
    qt_not_deployed {
        contains(S60_VERSION, 3.1)|contains(S60_VERSION, 3.2)|contains(S60_VERSION, 5.0) {
            sqlite.path = /sys/bin
            sqlite.files = sqlite3.dll
            DEPLOYMENT += sqlite
        }
    }
} else {
   win32:LIBS += -lws2_32
}

