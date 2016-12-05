
#-------------------------------------------------
#
# Project created by QtCreator 2015-03-18T17:27:39
#
#-------------------------------------------------

TARGET = moar-gl

QMAKE_CXXFLAGS += -std=c++11
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DEFINES += DEBUG

copydata.commands = $(COPY_DIR) $$PWD/settings.ini $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

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
    ../engine/material.cpp \
    ../engine/gui.cpp \
    ../engine/light.cpp \
    ../engine/rendersettings.cpp \
    ../engine/common/plane.cpp \
    ../engine/postprocess.cpp \
    ../engine/depthmap.cpp \
    ../engine/common/globals.cpp \
    ../engine/time.cpp \
    ../engine/framebuffer.cpp \
    ../engine/depthmapdir.cpp \
    ../engine/depthmappoint.cpp \
    ../engine/postframebuffer.cpp \
    ../engine/renderer.cpp \
    ../engine/gbuffer.cpp \
    ../engine/multisamplebuffer.cpp \
    ../engine/common/typemappings.cpp

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
    ../engine/material.h \
    ../engine/gui.h \
    ../engine/light.h \
    ../engine/rendersettings.h \
    ../engine/common/plane.h \
    ../engine/postprocess.h \
    ../engine/common/quad.h \
    ../engine/depthmap.h \
    ../engine/common/globals.h \
    ../engine/time.h \
    ../engine/framebuffer.h \
    ../engine/depthmapdir.h \
    ../engine/depthmappoint.h \
    ../engine/postframebuffer.h \
    ../engine/renderer.h \
    ../engine/gbuffer.h \
    ../engine/multisamplebuffer.h \
    ../engine/common/typemappings.h

INCLUDEPATH += $$PWD/../external/glm/

# GLFW
unix:!macx: LIBS += -L$$PWD/../external/glfw/src/ -lglfw3
unix:!macx: LIBS += -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL -ldl -pthread

INCLUDEPATH += $$PWD/../external/glfw/include/
DEPENDPATH += $$PWD/../external/glfw/include/

unix:!macx: PRE_TARGETDEPS += $$PWD/../external/glfw/src/libglfw3.a

# GLEW
unix:!macx: LIBS += -lGLEW

INCLUDEPATH += /usr/include/GL
DEPENDPATH += /usr/include/GL

# SOIL
unix:!macx: LIBS += -lSOIL

INCLUDEPATH += /usr/include/SOIL
DEPENDPATH += /usr/include/SOIL

# Assimp
unix:!macx: LIBS += -lassimp

INCLUDEPATH += $$PWD/../external/assimp/include
DEPENDPATH += $$PWD/../external/assimp/include


# AntTweakBar
unix:!macx: LIBS += -lAntTweakBar

INCLUDEPATH += /usr/include/AntTweakBar
DEPENDPATH += /usr/include/AntTweakBar

# Boost
#unix:!macx: LIBS += -lboost_system -lboost_filesystem

#INCLUDEPATH += /usr/local/include/boost/
#DEPENDPATH += /usr/local/include/boost/
