#-------------------------------------------------
#
# Project created by QtCreator 2015-03-18T17:27:39
#
#-------------------------------------------------

TARGET = moar-gl

CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

DISTFILES += \
    settings.ini \
    shaders/test.frag \
    shaders/test.vert

SOURCES += \
    main.cpp \
    myapp.cpp \
    ../engine/engine.cpp \
    ../engine/application.cpp \
    ../engine/shader.cpp \
    ../engine/model.cpp \
    ../engine/mesh.cpp \
    ../engine/object.cpp \
    ../engine/renderobject.cpp \
    ../engine/camera.cpp

HEADERS += \
    myapp.h \
    ../engine/engine.h \
    ../engine/application.h \
    ../engine/shader.h \
    ../engine/model.h \
    ../engine/mesh.h \
    ../engine/object.h \
    ../engine/renderobject.h \
    ../engine/camera.h

# GLEW
unix:!macx: LIBS += -L /usr/lib/x86_64-linux-gnu/ -lGLEW

INCLUDEPATH += /usr/include/GL
DEPENDPATH += /usr/include/GL

# GLFW
unix:!macx: LIBS += -L$$PWD/../glfw/src/ -lglfw3 -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL

INCLUDEPATH += $$PWD/../glfw/include/
DEPENDPATH += $$PWD/../glfw/include/

unix:!macx: PRE_TARGETDEPS += $$PWD/../glfw/src/libglfw3.a

# SOIL
unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/ -lSOIL

INCLUDEPATH += $$PWD/../../../../../usr/include/SOIL
DEPENDPATH += $$PWD/../../../../../usr/include/SOIL

# Assimp
unix:!macx: LIBS += -L$$PWD/../assimp/lib/ -lassimp

INCLUDEPATH += $$PWD/../assimp/include
DEPENDPATH += $$PWD/../assimp/include
