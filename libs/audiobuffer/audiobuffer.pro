TARGET = audiobuffer
CONFIG += c++11

# Set concurrentqueue header include path
INCLUDEPATH += $$PROJECT_DIR/libs/extern/concurrentqueue

HEADERS += audiobuffer.h \
    rawbuffer.h \
    ringbuffer.h
SOURCES += audiobuffer.cpp \
    rawbuffer.cpp \
    ringbuffer.cpp

include(../lib.pri)
