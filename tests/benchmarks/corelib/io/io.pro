TEMPLATE = subdirs
SUBDIRS = \
        qdir \
        qdiriterator \
        qfile \
        qfileinfo \
        qiodevice \
        qtemporaryfile

android: SUBDIRS -= qfileinfo


