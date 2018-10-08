#include "rawbuffer.h"
#include <iostream>

/*
 * Constructor
 */
template<class type>
RawBuffer<type>::RawBuffer(unsigned int format)
{
    rawAudioFormat = format;
    rawBuffer = NULL;
    clear();
}

/*
 * Destructor
 */
template<class type>
RawBuffer<type>::~RawBuffer()
{
    clear();
}

/*
 * Frees all pre-allocated memory
 */
template<class type>
void RawBuffer<type>::clear()
{
    rawBufferSize = 0;
    numberOfChannels = 0;
    rawFrames = QVector<type>();
    frames = QVector<double>();
    if( rawBuffer != NULL ) {
        delete rawBuffer;
        rawBuffer = NULL;
    }
}

/*
 * Allocates numberOfFrames frames for each numOfChannels, assignes em with given value.
 */
template<class type>
bool RawBuffer<type>::allocate(unsigned int numberOfFrames, unsigned int numOfChannels, double value)
{
    clear();

    unsigned int bufferSize = numberOfFrames * numOfChannels;
    if( bufferSize == 0 )
        return false;

    rawBufferSize = numberOfFrames;
    numberOfChannels = numOfChannels;
	frames = QVector<double>(bufferSize, value);
    rawFrames = QVector<type>(bufferSize, (type)value);
    rawBuffer = new moodycamel::ConcurrentQueue<type>(bufferSize,1,1);

    return true;
}

/*
 * Insert raw frames into concurrent queue.
 *
 * If dequeue true directly populates rawFrames buffer.
 * Otherwise call popRawDataFromQueue afterwards manually.
 */
template<class type>
bool RawBuffer<type>::insert(void* theFrames, unsigned int size, bool dequeue)
{
    type* input = (type*)theFrames;
    bool success = pushFramesToQueue(input, size);
    if( success && dequeue == true ) {
        success = success && grabFramesFromQueue();
    }
    return success;
}

/*
 * Thread save: Insert/push frames into concurrent queue (size = channelCount*hwBuffer)
 */
template<class type>
bool RawBuffer<type>::pushFramesToQueue(type* frames, unsigned int size)
{
    unsigned int numOfFrames = rawBufferSize*numberOfChannels;
    if( size > numOfFrames ) {
        std::cerr << "Reducing data to push from " << size << " to " << numOfFrames;
        size = numOfFrames;
    }
    return rawBuffer->enqueue_bulk(frames, size);
}

/*
 * Thread save: Moves/pops frames from concurrent queue into rawFrames vector
 */
template<class type>
bool RawBuffer<type>::grabFramesFromQueue()
{
    int retryCounter = 0;
    size_t framesCopied = 0;
    size_t missingFrames = rawFrames.size();
    while (missingFrames > 0 ) {
        framesCopied += rawBuffer->try_dequeue_bulk(rawFrames.data() + framesCopied*sizeof(type), missingFrames);
        missingFrames = rawBuffer->size_approx();
        if( missingFrames > 0 ) {
            retryCounter++;
        }
        else break;

        if( retryCounter > 10 ) {
            std::cerr << "try_dequeue_bulk failed, missing " << missingFrames << "/" << rawFrames.size();
            return false;
        }
    }

    for(int i=0; i<rawFrames.size(); i++){
        frames[i] = (double)rawFrames[i];
    }

    return framesCopied > 0;
}
