LIBS += -lEGL

INCLUDEPATH += $$PWD $$PWD/..

SOURCES +=  \
            $$PWD/qeglconvenience.cpp \
            $$PWD/qandroideglplatformcontext.cpp \
            $$PWD/qandroideglfswindow.cpp \
            $$PWD/qandroideglfswindowsurface.cpp \
            $$PWD/qandroideglfsscreen.cpp \
            $$PWD/qandroideglfspaintdevice.cpp

HEADERS +=  \
            $$PWD/qeglconvenience.h \
            $$PWD/qandroideglplatformcontext.h \
            $$PWD/qandroideglfswindow.h \
            $$PWD/qandroideglfswindowsurface.h \
            $$PWD/qandroideglfsscreen.h \
            $$PWD/qandroideglfspaintdevice.h
