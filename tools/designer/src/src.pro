TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    uitools \
    lib \
    components \
    designer

CONFIG(shared,shared|static):SUBDIRS += plugins

android|symbian|wince*: SUBDIRS = uitools
