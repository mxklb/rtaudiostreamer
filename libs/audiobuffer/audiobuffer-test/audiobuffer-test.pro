TARGET = audiobuffer-test
SOURCES += audiobuffer-test.cpp

# Declare this as test app
include(../../test.pri)

# Relative path to libs
customLibs = ../audiobuffer

# Load and link all libs
include(../../loadlibs.pri)
