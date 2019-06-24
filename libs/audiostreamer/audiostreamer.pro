TARGET = audiostreamer

CONFIG += qt
QT += core
CONFIG += c++11

LIBS += -lrtaudio
macx {
    INCLUDEPATH += /usr/local/include/
    INCLUDEPATH += /usr/local/include/rtaudio/
    LIBS += -L/usr/local/lib -lrtaudio
}

INCLUDEPATH += /usr/include/rtaudio/

SOURCES += audiostreamer.cpp \
    audioprocessing.cpp \
    audiocallback.cpp \
    streamsettings.cpp
HEADERS += audiostreamer.h \
    audioprocessing.h \
    streamsettings.h \
    audiocallback.h

include(../lib.pri)

# Relative path to custom libs
customLibs = ../audiobuffer/audiobuffer

# Load and link custom libs
include(../loadlibs.pri)
