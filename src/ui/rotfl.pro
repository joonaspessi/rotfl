#-------------------------------------------------
#
# Project created by QtCreator 2013-09-20T14:31:26
#
#-------------------------------------------------

QT       += core gui network quick qml widgets opengl


TARGET = rotfl
TEMPLATE = app

INCLUDEPATH += "..//"
QMAKE_CXXFLAGS += -std=c++0x

CONFIG += static

SOURCES += main.cpp\
        mainwindow.cpp \
    mapQGraphicsView.cpp \
    poiQGraphicsEllipseItem.cpp \
    ../../ext_lib/RooWifi_11b/roowifi.cpp \
    ../croi/roombaRoowifi.cpp \
    wallQGraphicsLineItem.cpp \
    ../croi/iRoomba.cpp \
    fleetManager.cpp \
    flogger.cpp \
    ../croi/roombaVirtual.cpp \
    atcQGraphicsRectItem.cpp

HEADERS  += mainwindow.h \
    ../croi/iRoomba.h \
    poiQGraphicsEllipseItem.h \
    mapQGraphicsView.h \
    ../../ext_lib/RooWifi_11b/roowifi.h \
    ../../ext_lib/RooWifi_11b/roomba.h \
    ../croi/roombaRoowifi.h \
    wallQGraphicsLineItem.h \
    uiUtils.h \
    fleetManager.h \
    flogger.h \
    ../croi/roombaVirtual.h \
    atcQGraphicsRectItem.h

FORMS    += mainwindow.ui

RESOURCES += \
    graphics.qrc


OTHER_FILES += \
    qml/gauges.qml
