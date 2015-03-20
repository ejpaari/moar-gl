#-------------------------------------------------
#
# Project created by QtCreator 2015-03-18T17:27:39
#
#-------------------------------------------------

TARGET = moar-gl

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

DISTFILES += \
    settings.ini

SOURCES += main.cpp

# GLEW
unix:!macx: LIBS += -L$$PWD/moar-gl-external/GLEW/ -lGLEW

INCLUDEPATH += $$PWD/moar-gl-external/GLEW/GL
DEPENDPATH += $$PWD/moar-gl-external/GLEW/GL

unix:!macx: PRE_TARGETDEPS += $$PWD/moar-gl-external/GLEW/libGLEW.a

# GLFW
unix:!macx: LIBS += -L$$PWD/moar-gl-external/GLFW/ -lglfw3 -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL

INCLUDEPATH += $$PWD/moar-gl-external/GLFW/include
DEPENDPATH += $$PWD/moar-gl-external/GLFW/include

unix:!macx: PRE_TARGETDEPS += $$PWD/moar-gl-external/GLFW/libglfw3.a

# SOIL
unix:!macx: LIBS += -L$$PWD/moar-gl-external/SOIL/ -lSOIL

INCLUDEPATH += $$PWD/moar-gl-external/SOIL/include
DEPENDPATH += $$PWD/moar-gl-external/SOIL/include

unix:!macx: PRE_TARGETDEPS += $$PWD/moar-gl-external/SOIL/libSOIL.a
