#include <algorithm>
#include "audiobuffer.h"

#include <iostream>


AudioBuffer::AudioBuffer()
{
    frameCounter = 0;
    ringBufferSize = 4096;
    rawAudioBuffer = NULL;
}

/*
 * Allocate one ringbuffer with size capatcity for each given channel.
 */
bool AudioBuffer::allocateRingbuffers(unsigned int size, QList<unsigned int> channels, double value)
{
    ringBufferSize = size;
    ringBufferContainer.clear();
    std::sort(channels.begin(), channels.end());
    activeChannelIds = channels;

    for( unsigned int i=0; i<numberOfChannels(); i++ ) {
        QVector<double> buffer(ringBufferSize, value);
        ringBufferContainer.push_back(buffer);
    }

    unsigned int rawBufferSize = numberOfChannels()*size;
    rawAudioFrames = QVector<signed int>(rawBufferSize, 0);

    if( rawAudioBuffer != NULL )
        delete rawAudioBuffer;
    rawAudioBuffer = new moodycamel::ConcurrentQueue<signed short>(rawBufferSize, 1, 1);

    //std::cout << " Size approx (before enqueue): " << rawAudioBuffer->size_approx() << std::endl;
    if( !rawAudioBuffer->enqueue_bulk(rawAudioFrames.data(), rawBufferSize) ) {
        //std::cout << " Size approx (after enqueue): " << rawAudioBuffer->size_approx() << std::endl;
        return false;
   /*     QVector<signed short> tester(rawBufferSize, 1);
        if( rawAudioBuffer->try_dequeue_bulk(tester.data(), rawBufferSize) == rawBufferSize) {
            std::cout << " Size approx (after dequeue): " << rawAudioBuffer->size_approx() << std::endl;
            foreach (signed short value, tester) {
                std::cout << value << ", " << std::endl;
            }
        } */
    }

    return !channels.isEmpty();
}

/*
 * Tries to remove/dequeue numOfFrames frames from the rawAudioBuffer.
 * -> While copying these frames into the rawAudioFrames vector!
 */
bool AudioBuffer::grabSharedFrames(unsigned int numOfFrames)
{
    if( numOfFrames == 0 || numberOfChannels() == 0 )
        return false;

    unsigned int size = numOfFrames * numberOfChannels();
    size_t count = rawAudioBuffer->try_dequeue_bulk(rawAudioFrames.data(), size);

    /*if( count != size ) {
        std::cout << "Frames missing" << std::endl;
        return false;
    }*/

    for(int i=0; i<10; i++)
        std::cout << rawAudioFrames.at(i) << ", ";
    std::cout << std::endl;

    return true;
}

/*
 * Returns the number of active channels.
 */
unsigned int AudioBuffer::numberOfChannels()
{
    return activeChannelIds.size();
}

/*
 *  Returns the channel id for the given channel out of activeChannelIds.
 */
unsigned int AudioBuffer::activeChannelId(unsigned int ch)
{
    unsigned int channel = ch;

    if( activeChannelIds.size() > 0 && (int)ch < activeChannelIds.size() )
        channel = activeChannelIds.at(ch);

    return channel;
}

/*
 * Shifts the ringbuffers delta frames
 */
bool AudioBuffer::rotateRingbuffers(unsigned int delta)
{
    if( delta > ringBufferSize ) {
        return false;
    }

    for( unsigned int ch=0; ch<numberOfChannels(); ch++ ) {
        QVector<double> *buffer = &ringBufferContainer[ch];
        std::rotate(buffer->begin(), buffer->begin() + (buffer->size()-delta), buffer->end());
    }

    return true;
}
