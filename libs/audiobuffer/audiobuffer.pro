TARGET = audiobuffer
CONFIG += c++11

include(audiobuffer.pri)

HEADERS += audiobuffer.h \
    rawbuffer.h \
    ringbuffer.h
SOURCES += audiobuffer.cpp \
    rawbuffer.cpp \
    ringbuffer.cpp

include(../lib.pri)
