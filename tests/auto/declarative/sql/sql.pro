load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
QT += sql script
macx:CONFIG -= app_bundle

SOURCES += tst_sql.cpp

# Define SRCDIR equal to test's source directory
DEFINES += SRCDIR=\\\"$$PWD\\\"
