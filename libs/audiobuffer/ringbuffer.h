#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QVector>

class RingBuffer {
public:
    RingBuffer();

    unsigned int ringBufferSize;
    QVector<unsigned int> channelIds;
    QVector<QVector<double> > bufferContainer;

    bool allocate(unsigned int size, QVector<unsigned int> channels = QVector<unsigned int>(), double value = 0.0);
    template <typename type> bool insert(QVector<type> *rawData, unsigned int numOfFrames, unsigned int rawChannelCount);

    bool rotateRingbuffers(unsigned int delta);
    static QVector<unsigned int> cleanChannels(QVector<unsigned int> channels);
};

/*
 * Rotate/shift actual content numOfFrames. Afterwards set first numOfFrames values from rawData.
 */
template <typename type>
bool RingBuffer::insert(QVector<type> *rawData, unsigned int numOfFrames, unsigned int rawChannelCount)
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
            (*buffer)[i] = (double)rawData->at(idx);
        }
    }
    return success;
}

#endif // RINGBUFFER_H
