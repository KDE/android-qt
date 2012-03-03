CONFIG += testcase

TARGET = tst_moc
QT += sql network svg testlib
SOURCES += tst_moc.cpp

#exists(/usr/include/boost/spirit.hpp) {
#    message("including boost headers in test")
#    DEFINES += PARSE_BOOST
#    # need to add explicitly so that it ends up in moc's search path
#    INCLUDEPATH += /usr/include
#}

INCLUDEPATH += testproject/include testproject

DEFINES += SRCDIR=\\\"$$PWD\\\"
cross_compile: DEFINES += MOC_CROSS_COMPILED

HEADERS += using-namespaces.h no-keywords.h task87883.h c-comments.h backslash-newlines.h oldstyle-casts.h \
           slots-with-void-template.h qinvokable.h namespaced-flags.h trigraphs.h \
           escapes-in-string-literals.h cstyle-enums.h qprivateslots.h gadgetwithnoenums.h \
           dir-in-include-path.h single_function_keyword.h task192552.h task189996.h \
           task234909.h task240368.h pure-virtual-signals.h
if(*-g++*|*-icc*|*-clang|*-llvm):!irix-*:!win32-*: HEADERS += os9-newlines.h win-newlines.h

contains(QT_CONFIG, script): QT += script
contains(QT_CONFIG, qt3support): QT += qt3support
contains(QT_CONFIG, dbus) {
    DEFINES += WITH_DBUS
    QT += dbus
}
