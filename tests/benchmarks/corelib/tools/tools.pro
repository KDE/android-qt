TEMPLATE = subdirs
SUBDIRS = \
        containers-associative \
        containers-sequential \
        qbytearray \
        qrect \
        qregexp \
        qstring \
        qstringbuilder \
        qstringlist \
        qvector

android: SUBDIRS -= qregexp \
                    qstringlist
