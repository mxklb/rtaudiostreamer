#ifndef RAWBUFFER_H
#define RAWBUFFER_H

#include "concurrentqueue.h"
#include <QVector>

template <class type>
class RawBuffer {
public:
    RawBuffer();
    ~RawBuffer();

    unsigned int rawBufferSize;
    unsigned int numberOfChannels;
    QVector<type> rawFrames;
    moodycamel::ConcurrentQueue<type>* rawBuffer;

    bool allocate(unsigned int size, unsigned int numOfChannels, type value = 0);
    bool insert(type* data, unsigned int size, bool dequeue = false);

    bool grabFramesFromQueue();

private:
    bool pushFramesToQueue(type* frames, unsigned int size);
    void clear();
};

#endif // RAWBUFFER_H
