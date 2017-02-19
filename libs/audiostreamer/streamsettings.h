#ifndef STREAMSETTINGS_H
#define STREAMSETTINGS_H

#include <RtAudio.h>

class StreamSettings {
public:
    StreamSettings();

    RtAudioFormat audioFormat;
    unsigned int hwSampleRate;
    unsigned int hwBufferSize;
    RtAudio::StreamParameters parameters;
    RtAudio::StreamOptions options;

    static double formatLimit(RtAudioFormat type);
    double formatLimit();
};

#endif // STREAMSETTINGS_H
