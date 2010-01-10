TEMPLATE      = subdirs
SUBDIRS       = mandelbrot \
                semaphores

!CONFIG(android) : SUBDIRS += waitconditions

# install
target.path = $$[QT_INSTALL_EXAMPLES]/threads
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS threads.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/threads
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
