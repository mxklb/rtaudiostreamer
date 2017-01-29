TARGET = audiostreamer

CONFIG += qt
QT += core

SOURCES += audiostreamer.cpp
SOURCES += audioprocessing.cpp
HEADERS += audiostreamer.h
HEADERS += audiocallback.h
HEADERS += audioprocessing.h
HEADERS += streamsettings.h

LIBS += -lrtaudio

# Relative path to libs
customLibs = ../audiobuffer/audiobuffer

# Load and link these custom libs
include(../loadlibs.pri)

include(../lib.pri)
