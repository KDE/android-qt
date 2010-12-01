TEMPLATE = subdirs

SUBDIRS += minimal


qpa:CONFIG(android): SUBDIRS += android android_mt android_sw
