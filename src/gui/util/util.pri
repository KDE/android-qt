# Qt util module

HEADERS += \
        util/qsystemtrayicon.h \
        util/qcompleter.h \
        util/qcompleter_p.h \
        util/qdesktopservices.h \
        util/qsystemtrayicon_p.h \
        util/qundogroup.h \
        util/qundostack.h \
        util/qundostack_p.h \
        util/qundoview.h

SOURCES += \
        util/qsystemtrayicon.cpp \
        util/qcompleter.cpp \
        util/qdesktopservices.cpp \
        util/qundogroup.cpp \
        util/qundostack.cpp \
        util/qundoview.cpp


win32 {
		SOURCES += \
				util/qsystemtrayicon_win.cpp
}

unix:x11 {
		SOURCES += \
				util/qsystemtrayicon_x11.cpp
}

embedded|embedded_lite {
		SOURCES += \
				util/qsystemtrayicon_qws.cpp
}

!embedded:!embedded_lite:!x11:mac {
		OBJECTIVE_SOURCES += util/qsystemtrayicon_mac.mm
}

symbian {
    LIBS += -lsendas2 -letext -lapmime
    contains(QT_CONFIG, s60): LIBS += -lplatformenv -lCommonUI
}
