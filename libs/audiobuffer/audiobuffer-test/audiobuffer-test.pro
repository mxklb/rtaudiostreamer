TARGET = audiobuffer-test
SOURCES += audiobuffer-test.cpp

# Set concurrentqueue header include path
INCLUDEPATH += $$PROJECT_DIR/libs/extern/concurrentqueue

include($$PROJECT_DIR/libs/test.pri)

# Relative path to libs
customLibs = ../audiobuffer

# Load and link all libs
include($$PROJECT_DIR/libs/loadlibs.pri)
