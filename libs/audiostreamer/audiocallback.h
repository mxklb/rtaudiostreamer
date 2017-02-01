#ifndef AUDIOCALLBACK_H
#define AUDIOCALLBACK_H

#include <math.h>
#include <QMutex>
#include "audiobuffer.h"

/*
 * Callback triggered from running RtAudio stream.
 *
 * Copies all activated channels (frames) into AudioBuffer class pointer.
 *
 * Note: Actually this will only succeed if RtAudio channels are stored interleaved!
 *
 * Which channel ids in the inputBuffer are copied are defined in streamBuffers
 * deviceChannelIds list. Per default the RtAudio stream must be opened with
 *
 * parameters.nChannels = highestChannelId - lowestChannelId + 1;
 * parameters.firstChannel = lowestChannelId;
 *
 * So only the at least needed channels are streamed!
 */
int audioStreamCallback( void *outputBuffer, void *inputBuffer,
                         unsigned int hwFrameCount, double streamTime,
                         RtAudioStreamStatus status, void *streamBuffers )
{
    Q_UNUSED(outputBuffer)
    Q_UNUSED(streamTime)

    if( status ) std::cout << "Stream over/underflow detected." << std::endl;
    if( hwFrameCount == 0 ) std::cout << "Zero frames detected." << std::endl;

    signed short *samples = (signed short*)inputBuffer;
    AudioBuffer *audioBuffers = (AudioBuffer*)streamBuffers;
    unsigned int numberOfChannels = audioBuffers->numberOfChannels();
    unsigned int lowestChannel = audioBuffers->activeChannelId(0);
    unsigned int numberOfFrames = hwFrameCount;

    if( numberOfChannels == 0 ) {
        std::cout << "Zero channels detected." << std::endl;
        return 0;
    }

    if( !audioBuffers->rotateRingbuffers(numberOfFrames) ) {
        numberOfFrames = audioBuffers->ringBufferSize;
    }

    for( unsigned int ch=0; ch<numberOfChannels; ch++ ) {
        unsigned int channelPos = audioBuffers->activeChannelId(ch) - lowestChannel;
        QVector<double> *buffer = &audioBuffers->ringBufferContainer[ch];
        for( unsigned int i=0; i<numberOfFrames; i++ ) {
            (*buffer)[i] = (double)samples[i * numberOfChannels + channelPos];
        }
    }

    audioBuffers->frameCounter += numberOfFrames;
    return 0;
}

#endif // AUDIOCALLBACK_H
