TARGET = audiobuffer
CONFIG += c++11

# Set concurrentqueue header include path
INCLUDEPATH += $$PROJECT_DIR/libs/concurrentqueue

HEADERS += audiobuffer.h
SOURCES += audiobuffer.cpp

include(../lib.pri)
