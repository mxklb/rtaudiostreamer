TARGET = audiobuffer-test
SOURCES += audiobuffer-test.cpp

include($$PROJECT_DIR/libs/test.pri)

# Relative path to libs
customLibs = ../audiobuffer

# Load and link all libs
include($$PROJECT_DIR/libs/loadlibs.pri)
