TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += __USE_MINGW_ANSI_STDIO=1
DEFINES += CSC_SRCDIR=\\\"../demo_imgui/\\\"

QMAKE_CXXFLAGS += -Wno-unused-function
QMAKE_CXXFLAGS += -Wno-missing-braces
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-missing-field-initializers



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
