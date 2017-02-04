#include "audiobuffer.h"

AudioBuffer::AudioBuffer() : frameCounter(0), ringBufferSize(4096)
{
    frameCounter = 0;
    ringBufferSize = 4096;
}

/*
 * Allocate one ringbuffer with size capatcity for each given channel.
 */
void AudioBuffer::allocateRingbuffers(unsigned int size, QList<unsigned int> channels, double value)
{
    ringBufferSize = size;
    ringBufferContainer.clear();
    std::sort(channels.begin(), channels.end());
    activeChannelIds = channels;

    for( unsigned int i=0; i<numberOfChannels(); i++ ) {
        QVector<double> buffer(ringBufferSize, value);
        ringBufferContainer.push_back(buffer);
    }
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
