#-------------------------------------------------
#
# Project created by QtCreator 2013-10-02T10:26:59
#
#-------------------------------------------------

QT       -= core gui

QMAKE_CXXFLAGS += -std=c++0x

TARGET = Croip
TEMPLATE = lib

DEFINES += CROIP_LIBRARY

SOURCES += croip.cpp \
    croippacket.cpp \
    croipsend.cpp \
    croipreceive.cpp

HEADERS += croip.h \
    croippacket.h \
    croipsend.h \
    croipreceive.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
11
