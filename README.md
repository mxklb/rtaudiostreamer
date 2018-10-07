# RtAudioStreamer

This is a C++ qmake app which uses [RtAudio](https://github.com/thestk/rtaudio) to process real time audio streams from connected audio hardware.

It's an example project how to use RtAudio for multi OS audio / soundcard monitoring.

[![Build Status](https://travis-ci.org/mxklb/rtaudiostreamer.svg?branch=master)](https://travis-ci.org/mxklb/rtaudiostreamer)
[![codecov](https://codecov.io/gh/mxklb/rtaudiostreamer/branch/master/graph/badge.svg)](https://codecov.io/gh/mxklb/rtaudiostreamer)
[![build status](https://gitlab.com/mxklb/rtaudiostreamer/badges/master/build.svg)](https://gitlab.com/mxklb/rtaudiostreamer/pipelines)
[![coverage report](https://gitlab.com/mxklb/rtaudiostreamer/badges/master/coverage.svg)](https://gitlab.com/mxklb/rtaudiostreamer/builds/artifacts/master/download?job=debug_tests)
[![GitLab license](https://img.shields.io/badge/MIT-license-blue.svg)](https://gitlab.com/mxklb/rtaudiostreamer/blob/master/LICENSE)

Download latest development version here
- [RtAudioStreamer-linux.x86_64.AppImage](https://gitlab.com/mxklb/rtaudiostreamer/builds/artifacts/master/download?job=deploy_appimage)
- [rtaudiostreamer.ubuntu18.04_amd64.deb](https://gitlab.com/mxklb/rtaudiostreamer/builds/artifacts/master/download?job=deploy_bionic)
- [rtaudiostreamer.ubuntu16.04_amd64.deb](https://gitlab.com/mxklb/rtaudiostreamer/builds/artifacts/master/download?job=deploy_xenial)
- [rtaudiostreamer.ubuntu14.04_amd64.deb](https://gitlab.com/mxklb/rtaudiostreamer/builds/artifacts/master/download?job=deploy_trusty)
- [RtAudioStreamer.macOS-10.13.dmg](https://rawgit.com/mxklb/rtaudiostreamer/osx-deploy/RtAudioStreamer.dmg)

## Build Dependencies
To succesfully build on Ubuntu:

    sudo apt-get install qt5-default qt5-qmake gdb librtaudio-dev

To succesfully build on OSX:

    brew update
    brew install qt5 rt-audio
    export PATH=$(brew --prefix)/opt/qt5/bin:$PATH

## Build Instructions

    qmake
    make
