#ifndef AUDIOCALLBACK_H
#define AUDIOCALLBACK_H

#include <RtAudio.h>

class AudioCallback {
public:
    static int interleaved( void *outputBuffer, void *inputBuffer,
                            unsigned int hwFrameCount, double streamTime,
                            RtAudioStreamStatus status, void *streamBuffers );
};

#endif // AUDIOCALLBACK_H
