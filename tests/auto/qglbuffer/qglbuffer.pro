############################################################
# Project file for autotest for file qglbuffer.h
############################################################

load(qttest_p4)
requires(contains(QT_CONFIG,opengl))
QT += opengl

SOURCES += tst_qglbuffer.cpp
