QT      +=  webkit network
HEADERS =   mainwindow.h
SOURCES =   main.cpp \
            mainwindow.cpp
RESOURCES = jquery.qrc

CONFIG += dll

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/fancybrowser
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/fancybrowser
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000CF6C
    TARGET.EPOCHEAPSIZE = 0�020000 0�4000000
    TARGET.CAPABILITY += NetworkServices
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

