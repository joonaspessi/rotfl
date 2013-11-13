#-------------------------------------------------
#
# Project created by QtCreator 2013-09-20T14:31:26
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rotfl
TEMPLATE = app

unix:INCLUDEPATH += "../"
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
        mainwindow.cpp \
    threadReader.cpp \
    ../croi/posixSerial.cpp \
    ../croi/roombaSerial.cpp \
    mapQGraphicsView.cpp \
    poiQGraphicsEllipseItem.cpp \
    ../../ext_lib/RooWifi_11b/roowifi.cpp \
    ../croi/roombaRoowifi.cpp \
    wallQGraphicsLineItem.cpp \
    ../croi/iRoomba.cpp \
    fleetManager.cpp \
    Flogger.cpp

HEADERS  += mainwindow.h \
    threadReader.h \
    ../croi/posixSerialUtils.h \
    ../croi/posixSerial.h \
    ../croi/iRoomba.h \
    ../croi/roombaSerial.h \
    ../croi/croiUtil.h \
    poiQGraphicsEllipseItem.h \
    mapQGraphicsView.h \
    ../../ext_lib/RooWifi_11b/roowifi.h \
    ../../ext_lib/RooWifi_11b/roomba.h \
    ../croi/roombaRoowifi.h \
    wallQGraphicsLineItem.h \
    uiUtils.h \
    fleetManager.h \
    Flogger.h

FORMS    += mainwindow.ui

RESOURCES += \
    graphics.qrc
