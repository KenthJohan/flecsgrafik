TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += __USE_MINGW_ANSI_STDIO=1
DEFINES += CSC_SRCDIR=\\\"../demo2/\\\"
#DEFINES += DEBUG

HEADERS += csc/csc_math.h
HEADERS += csc/csc_qf32.h
HEADERS += csc/csc_mf32.h
HEADERS += csc/csc_vf32.h
HEADERS += csc/csc_vf32_misc.h
HEADERS += csc/csc_vu32.h
HEADERS += csc/csc_f32.h
HEADERS += csc/csc_vf32_print.h
HEADERS += csc/csc_vf32_convolution.h
HEADERS += csc/csc_v3f32.h
HEADERS += csc/csc_v4f32.h
HEADERS += csc/csc_m3f32.h
HEADERS += csc/csc_m4f32.h
HEADERS += csc/csc_basic.h
HEADERS += csc/csc_gcam.h
HEADERS += ../flecs/flecs.h


QMAKE_CFLAGS += -Wno-unused-function
QMAKE_CFLAGS += -Wno-missing-braces
QMAKE_CFLAGS += -Wno-unused-parameter
QMAKE_CFLAGS += -Wno-missing-field-initializers

SOURCES += demo3.c
SOURCES += flecs.c

INCLUDEPATH += C:/msys64/mingw64/include/freetype2

#LIBS += -lnng
#LIBS += -lws2_32 -lwsock32 -lpthread

LIBS += -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32 -lfreetype


DEFINES += NNG_STATIC_LIB
LIBS += -Wl,-Bstatic
LIBS += -lnng
LIBS += -lws2_32 -lmswsock -ladvapi32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32
