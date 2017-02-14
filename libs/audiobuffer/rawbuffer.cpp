#include "rawbuffer.h"
#include <iostream>

/*
 * Constructor
 */
RawBuffer::RawBuffer()
{
    rawBuffer = NULL;
    rawBufferSize = 0;
    numberOfChannels = 0;
    rawFrames = QVector<signed short>();
}

/*
 * Destructor
 */
RawBuffer::~RawBuffer() {
    clear();
}

/*
 * Allocates numberOfFrames frames for each numOfChannels, assignes em with given value.
 */
bool RawBuffer::allocate(unsigned int numberOfFrames, unsigned int numOfChannels, signed short value)
{
    clear();

    unsigned int bufferSize = numberOfFrames * numOfChannels;
    if( bufferSize == 0 )
        return false;

    rawBufferSize = numberOfFrames;
    numberOfChannels = numOfChannels;
    rawFrames = QVector<signed short>(bufferSize, value);
    rawBuffer = new moodycamel::ConcurrentQueue<signed short>(bufferSize,1,1);

    return true;
}

/*
 * Frees all pre-allocated memory
 */
void RawBuffer::clear()
{
    rawBufferSize = 0;
    numberOfChannels = 0;
    rawFrames = QVector<signed short>();
    if( rawBuffer != NULL ) {
        delete rawBuffer;
        rawBuffer = NULL;
    }
}

/*
 * Insert raw frames into concurrent queue.
 *
 * If dequeue true directly populates rawFrames buffer.
 * Otherwise call popRawDataFromQueue afterwards manually.
 */
bool RawBuffer::insert(signed short* frames, unsigned int size, bool dequeue)
{
    bool success = pushFramesToQueue(frames, size);
    if( success && dequeue == true ) {
        success = success && grabFramesFromQueue();
    }
    return success;
}

/*
 * Thread save: Insert/push frames into concurrent queue (size = channelCount*hwBuffer)
 */
bool RawBuffer::pushFramesToQueue(signed short* frames, unsigned int size)
{
    unsigned int numOfFrames = rawBufferSize*numberOfChannels;
    if( size > numOfFrames ) {
        std::cerr << "Reducing data to push from " << size << " to " << numOfFrames << std::endl;
        size = numOfFrames;
    }
    return rawBuffer->enqueue_bulk(frames, size);
}

/*
 * Thread save: Moves/pops frames from concurrent queue to
 */
bool RawBuffer::grabFramesFromQueue()
{
    int retryCounter = 0;
    size_t framesCopied = 0;
    size_t missingFrames = rawFrames.size();
    while (missingFrames > 0 ) {
        framesCopied += rawBuffer->try_dequeue_bulk(rawFrames.data() + framesCopied*sizeof(signed short), missingFrames);
        missingFrames = rawBuffer->size_approx();
        if( missingFrames > 0 ) {
            retryCounter++;
        }
        else break;

        if( retryCounter > 10 ) {
            std::cerr << "try_dequeue_bulk failed, missing " << missingFrames << "/" << rawFrames.size() << std::endl;
            return false;
        }
    }
    return framesCopied > 0;
}
