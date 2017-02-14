#ifndef STREAMSETTINGS_H
#define STREAMSETTINGS_H

#include <RtAudio.h>
#include <QSettings>

class StreamSettings {
public:
    RtAudioFormat format;
    unsigned int hwSampleRate;
    unsigned int hwBufferSize;
    RtAudio::StreamParameters parameters;

    StreamSettings() {
        QSettings settings;
        hwSampleRate = settings.value("audiostreamer/hwSampleRate", 44100).toUInt();
        hwBufferSize = settings.value("audiostreamer/hwBufferSize", 256).toUInt();
        format = settings.value("audiostreamer/rtAudioFormat", (int)RTAUDIO_SINT16).toUInt();
    }
};

#endif // STREAMSETTINGS_H
