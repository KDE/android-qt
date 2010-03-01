TEMPLATE      = subdirs
SUBDIRS       = blockingfortuneclient \
                broadcastreceiver \
                broadcastsender \
                download \
                downloadmanager \
                fortuneclient \
                fortuneserver \
                qftp \
                http \
                loopback \
                threadedfortuneserver \
                googlesuggest \
                torrent \
                bearercloud \
                bearermonitor

# no QProcess
!vxworks:!qnx:SUBDIRS += network-chat

symbian: SUBDIRS = qftp

contains(QT_CONFIG, openssl):SUBDIRS += securesocketclient
contains(QT_CONFIG, openssl-linked):SUBDIRS += securesocketclient

# install
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS network.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/network
INSTALLS += sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
