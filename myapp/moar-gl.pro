
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
    shaders/normals.frag \
    shaders/normals.vert \
    shaders/textured_normals.frag \
    shaders/textured_normals.vert \
    shaders/diffuse.vert \
    ../shaders/diffuse.frag \
    ../shaders/normals.frag \
    ../shaders/diffuse.vert \
    ../shaders/normals.vert \
    ../shaders/ambient.frag \
    ../shaders/ambient.vert \
    ../shaders/skybox.frag \
    ../shaders/skybox.vert \
    ../shaders/specular.frag \
    ../shaders/specular.vert \
    ../shaders/normalmap.vert \
    ../shaders/normalmap.frag

SOURCES += \
    main.cpp \
    myapp.cpp \
    ../engine/engine.cpp \
    ../engine/application.cpp \
    ../engine/shader.cpp \
    ../engine/model.cpp \
    ../engine/mesh.cpp \
    ../engine/object.cpp \
    ../engine/camera.cpp \
    ../engine/input.cpp \
    ../engine/resourcemanager.cpp \
    ../engine/texture.cpp \
    ../engine/component.cpp \
    ../engine/renderer.cpp \
    ../engine/material.cpp \
    ../engine/gui.cpp \
    ../engine/light.cpp \
    ../engine/rendersettings.cpp \
    ../engine/globals.cpp

HEADERS += \
    myapp.h \
    ../engine/engine.h \
    ../engine/application.h \
    ../engine/shader.h \
    ../engine/model.h \
    ../engine/mesh.h \
    ../engine/object.h \
    ../engine/camera.h \
    ../engine/input.h \
    ../engine/resourcemanager.h \
    ../engine/texture.h \
    ../engine/component.h \
    ../engine/renderer.h \
    ../engine/material.h \
    ../engine/gui.h \
    ../engine/light.h \
    ../engine/rendersettings.h \
    ../engine/globals.h \
    ../engine/general.h

# GLFW
unix:!macx: LIBS += -L$$PWD/../glfw/src/ -lglfw3 -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL

INCLUDEPATH += $$PWD/../glfw/include/
DEPENDPATH += $$PWD/../glfw/include/

unix:!macx: PRE_TARGETDEPS += $$PWD/../glfw/src/libglfw3.a

# GLEW
unix:!macx: LIBS += -L/usr/lib/x86_64-linux-gnu/ -lGLEW

INCLUDEPATH += /usr/include/GL
DEPENDPATH += /usr/include/GL

# SOIL
unix:!macx: LIBS += -L/usr/lib/ -lSOIL

INCLUDEPATH += /usr/include/SOIL
DEPENDPATH += /usr/include/SOIL

# Assimp
unix:!macx: LIBS += -L$$PWD/../assimp/lib/ -lassimp

INCLUDEPATH += $$PWD/../assimp/include
DEPENDPATH += $$PWD/../assimp/include

# AntTweakBar
unix:!macx: LIBS += -L/usr/lib/ -lAntTweakBar

INCLUDEPATH += /usr/include/AntTweakBar
DEPENDPATH += /usr/include/AntTweakBar
