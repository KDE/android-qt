load(qttest_p4)
SOURCES  += tst_qnetworkconfigmanager.cpp
HEADERS  += ../qbearertestcommon.h

QT = core network

symbian {
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData
}

maemo6 {
    CONFIG += link_pkgconfig

    PKGCONFIG += conninet
}
