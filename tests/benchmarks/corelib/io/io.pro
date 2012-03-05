TEMPLATE = subdirs
SUBDIRS = \
        qdir \
        qdiriterator \
        qfile \
        #qfileinfo \    # FIXME: broken
        qiodevice \
        qtemporaryfile

android: SUBDIRS -= qfileinfo


