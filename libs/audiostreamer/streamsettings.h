#ifndef STREAMSETTINGS_H
#define STREAMSETTINGS_H

#include <RtAudio.h>
#include <QSettings>

class StreamSettings {
public:
    StreamSettings() : hwSampleRate(44100), hwBufferSize(256) {
        QSettings settings;
        hwSampleRate = settings.value("audiostreamer/hwSampleRate", 44100).toUInt();
        hwBufferSize = settings.value("audiostreamer/hwBufferSize", 256).toUInt();
    }
    RtAudio::StreamParameters parameters;
    unsigned int hwSampleRate;
    unsigned int hwBufferSize;
};

#endif // STREAMSETTINGS_H
