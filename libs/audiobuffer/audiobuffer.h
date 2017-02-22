#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include "rawbuffer.h"
#include "rawbuffer.cpp"
#include "ringbuffer.h"

class AudioBuffer {
public:
    AudioBuffer();
    ~AudioBuffer();

    double streamTimeStamp;
    unsigned long frameCounter;

    RingBuffer ringBuffer;
    RawBufferBase *rawBuffer;

    bool allocate(unsigned int ringBufferSize, QVector<unsigned int> channels = QVector<unsigned int>(),
                  unsigned int hwBufferSize = bufferdefaults::hwBufferSize, double value = 0.0);

    unsigned int numberOfChannels(bool raw = false);
    inline unsigned int ringBufferSize() { return ringBuffer.ringBufferSize; }
    inline unsigned int rawBufferSize() { return rawBuffer->rawBufferSize; }

    bool prepareRawBuffer(unsigned int format, unsigned int hwBufferLength);
    bool isFilled();
};

#endif // AUDIOBUFFER_H
