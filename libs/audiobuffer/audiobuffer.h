#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include "rawbuffer.h"
#include "rawbuffer.cpp"
#include "ringbuffer.h"

class AudioBuffer {
public:
    AudioBuffer();

    double streamTimeStamp;
    unsigned long frameCounter;

    RingBuffer ringBuffer;
    RawBufferBase *rawBuffer;

    bool allocate(unsigned int ringBufferSize, QVector<unsigned int> channels = QVector<unsigned int>(),
                  unsigned int hwBufferSize = RawBufferBase::defaultSize, double value = 0.0);

    bool switchRawAudioFormat(unsigned int format);
    unsigned int numberOfChannels(bool raw = false);
    unsigned int ringBufferSize() { return ringBuffer.ringBufferSize; }
    unsigned int rawBufferSize() { return rawBuffer->rawBufferSize; }

    bool isFilled();
};

#endif // AUDIOBUFFER_H
