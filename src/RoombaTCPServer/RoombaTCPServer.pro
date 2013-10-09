TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++0x
unix:INCLUDEPATH += "../"
SOURCES += main.cpp

LIBS += -lCroip

