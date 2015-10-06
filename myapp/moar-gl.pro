
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
    ../shaders/ambient.frag \
    ../shaders/ambient.vert \
    ../shaders/skybox.frag \
    ../shaders/skybox.vert \
    ../shaders/postproc/invert.frag \
    ../shaders/postproc/offset.frag \
    ../shaders/postproc/passthrough.frag \
    ../shaders/postproc/invert.vert \
    ../shaders/postproc/offset.vert \
    ../shaders/postproc/passthrough.vert \
    ../shaders/diffuse_point.frag \
    ../shaders/normalmap_point.frag \
    ../shaders/specular_point.frag \
    ../shaders/diffuse.vert \
    ../shaders/normalmap.vert \
    ../shaders/specular.vert \
    ../shaders/diffuse_dir.frag \
    ../shaders/normalmap_dir.frag \
    ../shaders/specular_dir.frag

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
    ../engine/globals.cpp \
    ../engine/common/plane.cpp \
    ../engine/postprocess.cpp \
    ../engine/framebuffer.cpp

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
    ../engine/common/math.h \
    ../engine/common/plane.h \
    ../engine/postprocess.h \
    ../engine/framebuffer.h

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
