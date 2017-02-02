TARGET = audiostreamer

CONFIG += qt
QT += core

SOURCES += audiostreamer.cpp \
    audioprocessing.cpp \
    audiocallback.cpp
HEADERS += audiostreamer.h \
    audioprocessing.h \
    streamsettings.h \
    audiocallback.h

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
