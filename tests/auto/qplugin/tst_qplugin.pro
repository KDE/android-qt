CONFIG += testcase

TARGET = tst_qplugin
QT = core testlib
SOURCES = tst_qplugin.cpp

wince*: {
    plugins.files = plugins/*
    plugins.path = plugins
    DEPLOYMENT += plugins
}

symbian: {
    rpDep.files = releaseplugin.dll debugplugin.dll
    rpDep.path = plugins
    DEPLOYMENT += rpDep dpDep
}
