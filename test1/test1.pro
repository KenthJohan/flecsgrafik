TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt



QMAKE_CFLAGS += -Wno-unused-function
QMAKE_CFLAGS += -Wno-missing-braces
QMAKE_CFLAGS += -Wno-unused-parameter
QMAKE_CFLAGS += -Wno-missing-field-initializers

SOURCES += test3.c
SOURCES += flecs.c

INCLUDEPATH += ../flecs
