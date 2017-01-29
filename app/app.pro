TEMPLATE = app
TARGET = RtAudioStreamer

CONFIG += console
CONFIG += qt
CONFIG += c++11
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += main.cpp

# Relative path to libs
customLibs = ../libs/audiobuffer/audiobuffer
customLibs += ../libs/audiostreamer/audiostreamer

# Load and link these custom libs
include(../libs/loadlibs.pri)

unix:!macx {
    target.files += $$OUT_PWD/$$TARGET
    target.path = $$[QT_INSTALL_PREFIX]/bin
    INSTALLS += target
}
