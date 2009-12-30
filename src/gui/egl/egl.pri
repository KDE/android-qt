CONFIG += egl

contains(CONFIG, android):INCLUDEPATH += ../plugins/graphicssystems/android

HEADERS += \
    egl/qegl_p.h \
    egl/qeglproperties_p.h

SOURCES += \
    egl/qegl.cpp \
    egl/qeglproperties.cpp

wince*: SOURCES += egl/qegl_wince.cpp

unix {
    embedded {
            SOURCES += egl/qegl_qws.cpp
    } else {
        embedded_lite {
            contains(CONFIG, android) {
                SOURCES += egl/qegl_android.cpp
            } else {
                SOURCES += egl/qegl_lite.cpp
            }
        } else {
            symbian {
                SOURCES += egl/qegl_symbian.cpp
            } else {
                SOURCES += egl/qegl_x11.cpp
            }
        }
    }
}
