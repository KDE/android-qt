TEMPLATE = lib
DESTDIR = com/nokia/TimeExample
TARGET  = qtimeexampleqmlplugin
CONFIG += qt plugin
QT += declarative
VERSION = 1.0.0

SOURCES += plugin.cpp

qdeclarativesources.files += \
    com/nokia/TimeExample/qdeclarativedir \
    com/nokia/TimeExample/center.png \
    com/nokia/TimeExample/clock.png \
    com/nokia/TimeExample/Clock.qml \
    com/nokia/TimeExample/hour.png \
    com/nokia/TimeExample/minute.png

qdeclarativesources.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins/com/nokia/TimeExample

sources.files += plugins.pro plugin.cpp plugins.qml
sources.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins

target.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins/com/nokia/TimeExample

INSTALLS += qdeclarativesources sources target

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
