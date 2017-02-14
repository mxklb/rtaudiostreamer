TARGET = audiostreamer

CONFIG += qt
QT += core
CONFIG += c++11

LIBS += -lrtaudio
macx {
    INCLUDEPATH += /usr/local/include/
    LIBS += -L/usr/local/lib -lrtaudio
}

# Set concurrentqueue header include path
INCLUDEPATH += $$PROJECT_DIR/libs/extern/concurrentqueue


SOURCES += audiostreamer.cpp \
    audioprocessing.cpp \
    audiocallback.cpp
HEADERS += audiostreamer.h \
    audioprocessing.h \
    streamsettings.h \
    audiocallback.h

include(../lib.pri)

# Relative path to custom libs
customLibs = ../audiobuffer/audiobuffer

# Load and link custom libs
include(../loadlibs.pri)
