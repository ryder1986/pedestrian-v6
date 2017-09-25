QT += core
QT -= gui
QT += network
TARGET = server
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    serverinforeporter.cpp

HEADERS += \
    server.h \
    common.h \
    serverinforeporter.h \
    videosrc.h \
    videohandler.h \
    camera.h \
    config.h \
    protocol.h

LIBS +=   -lopencv_core  -lopencv_highgui  -lopencv_objdetect -lopencv_imgproc -lopencv_ml -lopencv_video
DEFINES+=SERVER
