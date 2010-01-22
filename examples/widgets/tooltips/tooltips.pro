HEADERS       = shapeitem.h \
                sortingbox.h
SOURCES       = main.cpp \
                shapeItem.cpp \
                sortingbox.cpp
RESOURCES     = tooltips.qrc

CONFIG +=dll

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/tooltips
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS tooltips.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/tooltips
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
