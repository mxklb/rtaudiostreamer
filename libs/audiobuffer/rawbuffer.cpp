#include "rawbuffer.h"
#include <iostream>

RawBuffer::RawBuffer() {
    rawBufferSize = 256;
    numberOfChannels = 0;
    rawBuffer = NULL;
    rawFrames = QVector<signed short>();
}

/*
 * Allocates size*numOfChannels length buffer assigned with value
 */
bool RawBuffer::allocate(unsigned int size, unsigned int numOfChannels, signed short value)
{
    bool success = true;
    rawBufferSize = size;
    numberOfChannels = numOfChannels;
    unsigned int numOfFrames = size*numOfChannels;

    rawFrames.clear();
    if( rawBuffer != NULL )
        delete rawBuffer;

    if( numOfFrames > 0 ) {
        rawFrames = QVector<signed short>(numOfFrames, value);
        rawBuffer = new moodycamel::ConcurrentQueue<signed short>(numOfFrames);
    }
    else {
        rawFrames = QVector<signed short>();
        rawBuffer = NULL;
        success = false;
    }
    return success;
}

/*
 * Thread save: Insert/push frames into concurrent queue (size = channelCount*hwBuffer)
 */
bool RawBuffer::pushRawDataToQueue(signed short* data, unsigned int size)
{
    unsigned int numOfFrames = rawBufferSize*numberOfChannels;
    if( size > numOfFrames ) {
        std::cerr << "Reducing dada to push from " << size << " to " << numOfFrames << std::endl;
        size = numOfFrames;
    }
    return rawBuffer->try_enqueue_bulk(data, size);
}

/*
 * Thread save: Moves/pops frames from concurrent queue to
 */
bool RawBuffer::popRawDataFromQueue()
{
    size_t framesCopied = 0;
    size_t missingFrames = rawFrames.size();
    while (missingFrames > 0 ) {
        framesCopied += rawBuffer->try_dequeue_bulk(rawFrames.data() + framesCopied*sizeof(signed short), missingFrames);
        if( framesCopied == 0 ) {
            std::cerr << "try_dequeue_bulk failed, missing " << framesCopied << "/" << rawFrames.size() << std::endl;
            return false;
        }
        missingFrames = rawFrames.size() - framesCopied;
    }
    return true;
}
