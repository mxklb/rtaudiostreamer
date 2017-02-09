#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <QVector>
#include <QList>

#include "concurrentqueue.h"

/*
 * This class handles ring buffers for multi channel audio buffering.
 */
class AudioBuffer
{
 public:
    AudioBuffer();

    double timeStamp;
    unsigned long frameCounter;
    unsigned int ringBufferSize;
    QList<unsigned int> activeChannelIds;
    QList<QVector<double> > ringBufferContainer;

    moodycamel::ConcurrentQueue<signed short>* rawAudioBuffer;
    QVector<signed int> rawAudioFrames;

    bool allocateRingbuffers(unsigned int size, QList<unsigned int> channels = QList<unsigned int>(), double value = 0.0);
    bool rotateRingbuffers(unsigned int delta);
    unsigned int activeChannelId(unsigned int ch);
    unsigned int numberOfChannels();

    bool grabSharedFrames(unsigned int numOfFrames);
};

#endif // AUDIOBUFFER_H
