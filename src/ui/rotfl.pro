#-------------------------------------------------
#
# Project created by QtCreator 2013-09-20T14:31:26
#
#-------------------------------------------------

QT       += core gui network quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rotfl
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x

# Initial serial implementation (not in use) is for Linux only
unix {
    INCLUDEPATH += \
        "../../ext_lib/RooWifi_11b" \
        "../croi"
    SOURCES += \
        ../croi/posixSerial.cpp \
        ../croi/roombaSerial.cpp
    HEADERS += \
        ../croi/posixSerialUtils.h \
        ../croi/posixSerial.h \
        ../croi/roombaSerial.h
}
win32 {
    INCLUDEPATH += \
        "..\..\ext_lib\RooWifi_11b" \
        "..\croi"
}

SOURCES += \
    ../../ext_lib/RooWifi_11b/roowifi.cpp \
    ../croi/iRoomba.cpp \
    ../croi/roombaRoowifi.cpp \
    ../croi/roombaVirtual.cpp \
    atcQGraphicsRectItem.cpp \
    fleetManager.cpp \
    flogger.cpp \
    main.cpp \
    mainwindow.cpp \
    mapQGraphicsView.cpp \
    poiQGraphicsEllipseItem.cpp \
    wallQGraphicsLineItem.cpp

HEADERS  += \
    ../../ext_lib/RooWifi_11b/roowifi.h \
    ../../ext_lib/RooWifi_11b/roomba.h \
    ../croi/croiUtil.h \
    ../croi/iRoomba.h \
    ../croi/roombaRoowifi.h \
    ../croi/roombaVirtual.h \
    atcQGraphicsRectItem.h \
    fleetManager.h \
    flogger.h \
    mainwindow.h \
    mapQGraphicsView.h \
    poiQGraphicsEllipseItem.h \
    uiUtils.h \
    wallQGraphicsLineItem.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    graphics.qrc

OTHER_FILES += \
    qml/gauges.qml
