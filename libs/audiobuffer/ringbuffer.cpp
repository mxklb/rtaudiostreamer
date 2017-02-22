#include <QSettings>
#include <algorithm>
#include "ringbuffer.h"

/*
 * Constructor
 */
RingBuffer::RingBuffer()
{
    QSettings settings;
    allocate(settings.value("audiostreamer/processingBufferSize", bufferdefaults::ringBufferSize).toUInt());
}

/*
 * Sorts and removes dublicate entries in the given vector and returns it cleaned.
 */
QVector<unsigned int> RingBuffer::cleanChannels(QVector<unsigned int> channels)
{
    std::sort(channels.begin(), channels.end());
    auto last = std::unique(channels.begin(), channels.end());
    channels.erase(last, channels.end());
    return channels;
}

/*
 * Allocates size*channels.size() length buffer assigned with value. Sorts channels!
 */
bool RingBuffer::allocate(unsigned int size, QVector<unsigned int> channels, double value)
{
    bufferContainer.clear();
    ringBufferSize = size;
    channelIds = channels;
    if( channelIds.size() > 0 ) {
        channelIds = cleanChannels(channels);
        for( int ch=0; ch<channelIds.size(); ch++ ) {
            QVector<double> buffer(ringBufferSize, value);
            bufferContainer.push_back(buffer);
        }
    }
    return !bufferContainer.isEmpty();
}

/*
 * Rotate/shift actual content numOfFrames. Afterwards set first numOfFrames values from rawData.
 */
bool RingBuffer::insert(QVector<double> *rawData, unsigned int numOfFrames, unsigned int rawChannelCount)
{
    unsigned int lowestChannelId = channelIds.first();
    unsigned int numOfChannels = channelIds.size();
    bool success = true;

    if( !rotateRingbuffers(numOfFrames) ) {
        numOfFrames = ringBufferSize;
        success = false;
    }

    for( unsigned int ch=0; ch<numOfChannels; ch++ ) {
        unsigned int channelId = channelIds.at(ch) - lowestChannelId;
        QVector<double> *buffer = &bufferContainer[ch];
        for( unsigned int i=0; i<numOfFrames; i++ ) {
            int idx = i * rawChannelCount + channelId;
            if( idx >= rawData->size() ) break;
            (*buffer)[i] = rawData->at(idx);
        }
    }
    return success;
}

/*
 * Rotate all ringbuffers from left..right delta frames.
 */
bool RingBuffer::rotateRingbuffers(unsigned int delta)
{
    if( delta > ringBufferSize ) {
        return false;
    }
    for( int ch=0; ch<channelIds.size(); ch++ ) {
        QVector<double> *buffer = &bufferContainer[ch];
        std::rotate(buffer->begin(), buffer->begin() + (buffer->size()-delta), buffer->end());
    }
    return true;
}
