TEMPLATE = subdirs

SUBDIRS  = lrelease

!cross_compile: SUBDIRS  += \
    lupdate \
    lconvert

!cross_compile:!no-png:!contains(QT_CONFIG, no-gui):SUBDIRS += linguist
