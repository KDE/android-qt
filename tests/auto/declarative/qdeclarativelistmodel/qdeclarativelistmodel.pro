load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
QT += script
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativelistmodel.cpp
