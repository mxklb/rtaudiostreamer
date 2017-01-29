#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <algorithm>
#include <QVector>
#include <QList>

/*
 * This class handles ring buffers for multi channel audio buffering.
 */
class AudioBuffer
{
 public:
    AudioBuffer() : frameCounter(0), ringBufferSize(4096) {}

    unsigned long frameCounter;
    unsigned int ringBufferSize;
    QList<unsigned int> activeChannelIds;
    QList<QVector<double> > ringBufferContainer;

    /*
     * Allocate one ringbuffer with size capatcity for each given channel.
     */
    void allocateRingbuffers(unsigned int size, QList<unsigned int> channels = QList<unsigned int>()) {
        ringBufferSize = size;
        ringBufferContainer.clear();
        std::sort(channels.begin(), channels.end());
        activeChannelIds = channels;
        for( unsigned int i=0; i<numberOfChannels(); i++ ) {
            QVector<double> buffer(ringBufferSize, 0.0);
            ringBufferContainer.push_back(buffer);
        }
    }

    /*
     * Returns the number of active channels.
     */
    unsigned int numberOfChannels() {
        return activeChannelIds.size();
    }

    /*
     *  Returns the channel id for the given channel out of activeChannelIds.
     */
    unsigned int activeChannelId(unsigned int ch) {
        unsigned int channel = ch;
        if( activeChannelIds.size() > 0 && (int)ch < activeChannelIds.size() )
            channel = activeChannelIds.at(ch);
        return channel;
    }

    /*
     * Shifts the ringbuffers delta frames
     */
    bool rotateRingbuffers(unsigned int delta) {
        if( delta > ringBufferSize ) {
            return false;
        }
        for( unsigned int ch=0; ch<numberOfChannels(); ch++ ) {
            QVector<double> *buffer = &ringBufferContainer[ch];
            std::rotate(buffer->begin(), buffer->begin() + (buffer->size()-delta), buffer->end());
        }
        return true;
    }
};

#endif // AUDIOBUFFER_H
