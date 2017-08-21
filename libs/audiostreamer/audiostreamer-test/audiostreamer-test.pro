TARGET = audiostreamer-test
SOURCES += \
    audiostreamer-test.cpp \
    audiocallback-test.cpp \
    audioprocessing-test.cpp

# Declare this as test app
include(../../test.pri)

# Relative path to libs
customLibs = ../../audiobuffer/audiobuffer
customLibs += ../../audiostreamer/audiostreamer

# Load and link all libs
include(../../loadlibs.pri)
