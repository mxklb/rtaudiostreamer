#ifndef RAWBUFFER_H
#define RAWBUFFER_H

#include "concurrentqueue.h"
#include <QVector>

/*
 * Base interface for RawBuffer
 */
class RawBufferBase {
public:
    QVector<double> frames;
    unsigned int rawBufferSize;
    unsigned int numberOfChannels;

    virtual bool allocate(unsigned int size, unsigned int numOfChannels, double value = 0) = 0;
    virtual bool insert(void* data, unsigned int size, bool dequeue = false) = 0;
    virtual bool grabFramesFromQueue() = 0;
};

/*
 * Templated raw buffer container
 */
template <class type>
class RawBuffer : public RawBufferBase {
public:
    RawBuffer();
    ~RawBuffer();

    QVector<type> rawFrames;
    moodycamel::ConcurrentQueue<type>* rawBuffer;

    bool allocate(unsigned int size, unsigned int numOfChannels, double value = 0);
    bool insert(void* data, unsigned int size, bool dequeue = false);
    bool grabFramesFromQueue();

private:
    bool pushFramesToQueue(type* frames, unsigned int size);
    void clear();
};

/*
 * Simple buffer allocator
 */
class RawBufferFactory {
public:
    static RawBufferBase* createBuffer(unsigned int rawFormat) {
        RawBufferBase* buffer = NULL;
        switch( rawFormat ) {
            case 0x1: buffer = (RawBufferBase*)new RawBuffer<qint8>(); break;
            case 0x2: buffer = (RawBufferBase*)new RawBuffer<qint16>(); break;
            case 0x8: buffer = (RawBufferBase*)new RawBuffer<qint32>(); break;
            case 0x10: buffer = (RawBufferBase*)new RawBuffer<float>(); break;
            case 0x20: buffer = (RawBufferBase*)new RawBuffer<double>(); break;
            default: buffer = (RawBufferBase*)new RawBuffer<qint16>(); break;
        }
        return buffer;
    }
};

#endif // RAWBUFFER_H
