TEMPLATE = app
TARGET = RtAudioStreamer

CONFIG += console
CONFIG += qt
CONFIG += c++11
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += main.cpp

# Relative path to custom libs
customLibs = ../libs/audiobuffer/audiobuffer
customLibs += ../libs/audiostreamer/audiostreamer

# Load and link these custom libs
include(../libs/loadlibs.pri)

# Set application icon
ICON = ../img/RtAudioStreamer.svg
macx: ICON = ../img/logo.icns
win32: RC_ICONS += ../img/logo.ico

RESOURCES += app.qrc

unix:!macx {
    shortcutfiles.files = ../pkgs/appimage/rtaudiostreamer.desktop
    shortcutfiles.path = $$[QT_INSTALL_PREFIX]/share/applications/
    INSTALLS += shortcutfiles

    data.files += ../img/RtAudioStreamer.svg
    data.path = $$[QT_INSTALL_PREFIX]/share/pixmaps/
    INSTALLS += data

    target.files += $$OUT_PWD/$$TARGET
    target.path = $$[QT_INSTALL_PREFIX]/bin
    INSTALLS += target
}
