load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_layouts.cpp
macx:CONFIG -= app_bundle

# Define SRCDIR equal to test's source directory
DEFINES += SRCDIR=\\\"$$PWD\\\"
