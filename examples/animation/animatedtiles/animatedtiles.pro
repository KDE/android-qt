SOURCES = main.cpp
RESOURCES = animatedtiles.qrc
CONFIG += dll

# install
target.path = $$[QT_INSTALL_EXAMPLES]/animation/animatedtiles
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS animatedtiles.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/animation/animatedtiles
INSTALLS += target sources
