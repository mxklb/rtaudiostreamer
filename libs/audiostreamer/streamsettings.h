#ifndef STREAMSETTINGS_H
#define STREAMSETTINGS_H

#include <RtAudio.h>

class StreamSettings {
public:
    StreamSettings();

    RtAudioFormat format;
    unsigned int hwSampleRate;
    unsigned int hwBufferSize;
    RtAudio::StreamParameters parameters;

    static double formatLimit(RtAudioFormat type);
    double formatLimit();
};

#endif // STREAMSETTINGS_H
