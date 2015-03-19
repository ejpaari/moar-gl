#-------------------------------------------------
#
# Project created by QtCreator 2015-03-18T17:27:39
#
#-------------------------------------------------

TARGET = moar-gl

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

# GLEW
unix:!macx: LIBS += -L$$PWD/../../../../usr/lib64/ -lGLEW

INCLUDEPATH += $$PWD/../../../../usr/include/GL
DEPENDPATH += $$PWD/../../../../usr/include/GL

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../usr/lib64/libGLEW.a

# GLFW
unix:!macx: LIBS += -L$$PWD/../../../../usr/local/lib/ -lglfw3 -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL

INCLUDEPATH += $$PWD/../../../../usr/local/include/GLFW
DEPENDPATH += $$PWD/../../../../usr/local/include/GLFW

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/libglfw3.a

DISTFILES += \
    settings.ini
