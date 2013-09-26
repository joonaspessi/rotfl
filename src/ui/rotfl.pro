#-------------------------------------------------
#
# Project created by QtCreator 2013-09-20T14:31:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rotfl
TEMPLATE = app

unix:INCLUDEPATH += "../"

SOURCES += main.cpp\
        mainwindow.cpp \
    threadReader.cpp \
    ../croi/posixSerial.cpp

HEADERS  += mainwindow.h \
    threadReader.h \
    ../croi/posixSerialUtils.h \
    ../croi/posixSerial.h

FORMS    += mainwindow.ui
