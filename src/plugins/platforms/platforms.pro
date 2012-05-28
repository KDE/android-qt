TEMPLATE = subdirs

qpa:CONFIG(android): SUBDIRS += android
else:SUBDIRS += minimal
contains(QT_CONFIG, wayland) {
    SUBDIRS += wayland
}

qnx {
    SUBDIRS += blackberry
}
