TEMPLATE      = subdirs
SUBDIRS       = \
              elasticnodes \
              collidingmice \
              padnavigator \
	      basicgraphicslayouts

!symbian: SUBDIRS += \
              diagramscene \
              dragdroprobot \
              anchorlayout \
              simpleanchorlayout \
              weatheranchorlayout

!symbian: !CONFIG(android) : SUBDIRS += flowlayout

contains(QT_CONFIG, qt3support):SUBDIRS += portedcanvas portedasteroids
contains(DEFINES, QT_NO_CURSOR)|contains(DEFINES, QT_NO_DRAGANDDROP): SUBDIRS -= dragdroprobot

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS graphicsview.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
