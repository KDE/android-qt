# Qt kernel library base module

HEADERS +=  \
	global/qglobal.h \
	global/qnamespace.h \
        global/qendian.h \
        global/qnumeric_p.h \
        global/qnumeric.h

SOURCES += \
	global/qglobal.cpp \
        global/qlibraryinfo.cpp \
	global/qmalloc.cpp \
        global/qnumeric.cpp

nacl {
    SOURCES += global/qnaclunimplemented.cpp
}

# qlibraryinfo.cpp includes qconfig.cpp
INCLUDEPATH += $$QT_BUILD_TREE/src/corelib/global

# Only used on platforms with CONFIG += precompile_header
PRECOMPILED_HEADER = global/qt_pch.h

linux*:!static:!linux-armcc:!linux-gcce {
   QMAKE_LFLAGS += -Wl,-e,qt_core_boilerplate
   prog=$$quote(if (/program interpreter: (.*)]/) { print $1; })
   DEFINES += ELF_INTERPRETER=\\\"$$system(readelf -l /bin/ls | perl -n -e \'$$prog\')\\\"
}
