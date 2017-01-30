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

    void allocateRingbuffers(unsigned int size, QList<unsigned int> channels = QList<unsigned int>());
    bool rotateRingbuffers(unsigned int delta);
    unsigned int activeChannelId(unsigned int ch);
    unsigned int numberOfChannels();
};

#endif // AUDIOBUFFER_H
