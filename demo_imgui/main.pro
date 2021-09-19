TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += __USE_MINGW_ANSI_STDIO=1

QMAKE_CFLAGS += -Wno-unused-function
QMAKE_CFLAGS += -Wno-missing-braces
QMAKE_CFLAGS += -Wno-unused-parameter
QMAKE_CFLAGS += -Wno-missing-field-initializers



SOURCES += main.cpp
SOURCES += libs/flecs.c
SOURCES += libs/imgui.cpp
SOURCES += libs/imgui_draw.cpp
SOURCES += libs/imgui_tables.cpp
SOURCES += libs/imgui_widgets.cpp
SOURCES += libs/imgui_demo.cpp
SOURCES += libs/imgui_impl_sdl.cpp
SOURCES += libs/imgui_impl_opengl3.cpp
INCLUDEPATH += libs
LIBS += -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32 -lfreetype
