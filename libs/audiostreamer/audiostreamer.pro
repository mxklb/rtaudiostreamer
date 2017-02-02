TARGET = audiostreamer

CONFIG += qt
QT += core

SOURCES += audiostreamer.cpp
SOURCES += audioprocessing.cpp
HEADERS += audiostreamer.h
HEADERS += audiocallback.h
HEADERS += audioprocessing.h
HEADERS += streamsettings.h

include(../lib.pri)

LIBS += -lrtaudio
macx { 
    INCLUDEPATH += /usr/local/include/
    LIBS += -L/usr/local/lib -lrtaudio
}

# Relative path to custom libs
customLibs = ../audiobuffer/audiobuffer

# Load and link custom libs
include(../loadlibs.pri)
