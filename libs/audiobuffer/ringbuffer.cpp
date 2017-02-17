#include <QSettings>
#include <algorithm>
#include "ringbuffer.h"

/*
 * Constructor
 */
RingBuffer::RingBuffer() {
    QSettings settings;
    allocate(settings.value("audiostreamer/processingBufferSize", 8192).toUInt());
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
