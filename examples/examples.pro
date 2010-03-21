TEMPLATE      = subdirs
SUBDIRS       = \
                animation \
                desktop \
                dialogs \
                draganddrop \
                effects \
                graphicsview \
                ipc \
                itemviews \
                layouts \
                linguist \
                mainwindows \
                network \
                painting \
                richtext \
                sql \
                statemachine \
                threads \
                tools \
                tutorials \
                widgets \
                uitools \
                xml \
                multitouch \
                gestures

!CONFIG(android) : SUBDIRS += qtconcurrent

symbian: SUBDIRS = \
                graphicsview \
                itemviews \
                network \
                painting \
                widgets \
                draganddrop \
                mainwindows \
                sql \
                uitools \
                animation \
                gestures \
                xml

contains(QT_CONFIG, multimedia) {
    SUBDIRS += multimedia
}

contains(QT_CONFIG, script): SUBDIRS += script

contains(QT_CONFIG, phonon):!static: SUBDIRS += phonon
embedded:SUBDIRS += qws
!wince*:!symbian: {
    !contains(QT_EDITION, Console):contains(QT_BUILD_PARTS, tools):SUBDIRS += designer
    contains(QT_BUILD_PARTS, tools):SUBDIRS += qtestlib help
} else {
    contains(QT_BUILD_PARTS, tools):SUBDIRS += qtestlib
}
contains(QT_CONFIG, opengl): SUBDIRS += opengl
contains(QT_CONFIG, openvg): SUBDIRS += openvg
contains(QT_CONFIG, dbus): SUBDIRS += dbus
win32: SUBDIRS += activeqt
contains(QT_CONFIG, xmlpatterns): SUBDIRS += xmlpatterns
contains(DEFINES, QT_NO_CURSOR): SUBDIRS -= mainwindows

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]
INSTALLS += sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
