#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include "rawbuffer.h"
#include "ringbuffer.h"

class AudioBuffer {
public:
    AudioBuffer();

    double streamTimeStamp;
    unsigned long frameCounter;

    RawBuffer rawBuffer;
    RingBuffer ringBuffer;

    bool allocate(unsigned int ringBufferSize, QVector<unsigned int> channels = QVector<unsigned int>(),
                  unsigned int hwBufferSize = 256, double value = 0.0);

    unsigned int numberOfChannels(bool raw = false);
    unsigned int ringBufferSize() { return ringBuffer.ringBufferSize; }
    unsigned int rawBufferSize() { return rawBuffer.rawBufferSize; }
};

#endif // AUDIOBUFFER_H
