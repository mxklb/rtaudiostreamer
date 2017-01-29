TARGET = audiostreamer-test
SOURCES += main.cpp

include($$PROJECT_DIR/libs/test.pri)

macx { 
    INCLUDEPATH += /usr/local/include/
    LIBS += -L/usr/local/lib -lrtaudio
}

# Relative path to libs
customLibs = ../../audiobuffer/audiobuffer
customLibs += ../../audiostreamer/audiostreamer

# Load and link all libs
include($$PROJECT_DIR/libs/loadlibs.pri)
