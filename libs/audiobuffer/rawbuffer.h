#ifndef RAWBUFFER_H
#define RAWBUFFER_H

#include "concurrentqueue.h"
#include <QVector>

class RawBuffer {
public:
    RawBuffer();

    unsigned int rawBufferSize;
    unsigned int numberOfChannels;
    QVector<signed short> rawFrames;
    moodycamel::ConcurrentQueue<signed short>* rawBuffer;

    bool allocate(unsigned int size, unsigned int numOfChannels, signed short value = 0);

    bool pushRawDataToQueue(signed short* data, unsigned int size);
    bool popRawDataFromQueue();
};

#endif // RAWBUFFER_H
