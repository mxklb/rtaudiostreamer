TARGET = audiostreamer-test
SOURCES += \
    audiostreamer-test.cpp \
    audiocallback-test.cpp \
    audioprocessing-test.cpp

include($$PROJECT_DIR/libs/test.pri)

# Relative path to libs
customLibs = ../../audiobuffer/audiobuffer
customLibs += ../../audiostreamer/audiostreamer

# Load and link all libs
include($$PROJECT_DIR/libs/loadlibs.pri)
