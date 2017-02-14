#ifndef RAWBUFFER_H
#define RAWBUFFER_H

#include "concurrentqueue.h"
#include <QVector>

class RawBuffer {
public:
    RawBuffer();
    ~RawBuffer();

    unsigned int rawBufferSize;
    unsigned int numberOfChannels;
    QVector<signed short> rawFrames;
    moodycamel::ConcurrentQueue<signed short>* rawBuffer;

    bool allocate(unsigned int size, unsigned int numOfChannels, signed short value = 0);
    bool insert(signed short* data, unsigned int size, bool dequeue = false);

    bool grabFramesFromQueue();
private:
    bool pushFramesToQueue(signed short* frames, unsigned int size);
    void clear();
};

#endif // RAWBUFFER_H
