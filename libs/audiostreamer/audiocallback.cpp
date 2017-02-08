#include <math.h>
#include <QMutex>
#include "audiobuffer.h"
#include "audiocallback.h"

/*
 * Callback triggered from running RtAudio stream.
 *
 * Copies all activated channels (frames) into AudioBuffer class pointer.
 *
 * Note: This will only succeed if RtAudio channels are stored interleaved!
 *
 * Which channel ids in the inputBuffer are copied are defined in streamBuffers
 * deviceChannelIds list. Per default the RtAudio stream must be opened with
 *
 * parameters.nChannels = highestChannelId - lowestChannelId + 1;
 * parameters.firstChannel = lowestChannelId;
 *
 * in advance. So only the at least needed channels are streamed!
 */
int AudioCallback::interleaved( void *outputBuffer, void *inputBuffer, unsigned int hwFrameCount,
                                double streamTime, RtAudioStreamStatus status, void *streamBuffers )
{
    Q_UNUSED(outputBuffer)
    Q_UNUSED(streamTime)

    if( status ) std::cout << "Stream over/underflow detected." << std::endl;
    if( hwFrameCount == 0 ) std::cout << "Zero frames detected." << std::endl;

    signed short *samples = (signed short*)inputBuffer;
    AudioBuffer *audioBuffers = (AudioBuffer*)streamBuffers;
    unsigned int numberOfChannels = audioBuffers->numberOfChannels();

    if( numberOfChannels == 0 ) {
        std::cout << "Zero channels detected." << std::endl;
        return 0;
    }

    if( !audioBuffers->rawAudioBuffer->try_enqueue_bulk(samples, hwFrameCount*numberOfChannels) ) {
        std::cout << "try_enqueue_bulk fails: " << audioBuffers->rawAudioBuffer->size_approx() << std::endl;
        // Wake ringbuffer filling thread!

    }
    audioBuffers->frameCounter += hwFrameCount;
    //std::cout << audioBuffers->rawAudioBuffer->size_approx() << std::endl;

    return 0;
}
