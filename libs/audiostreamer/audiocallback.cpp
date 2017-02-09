#include <math.h>
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
                                double streamTime, RtAudioStreamStatus status, void *audioStreamer )
{
    Q_UNUSED(outputBuffer)

    if( status ) std::cerr << "Stream over/underflow detected." << std::endl;
    if( hwFrameCount == 0 ) std::cerr << "Zero frames detected." << std::endl;

    signed short *samples = (signed short*)inputBuffer;
    AudioStreamer *streamer = (AudioStreamer*)audioStreamer;
    AudioBuffer *audioBuffers = streamer->getAudioBuffers();
    unsigned int numberOfChannels = audioBuffers->numberOfChannels();

    if( numberOfChannels == 0 ) {
        std::cerr << "Zero channels detected." << std::endl;
        return 0;
    }

    if( !audioBuffers->rawAudioBuffer->try_enqueue_bulk(samples, hwFrameCount*numberOfChannels) ) {
        std::cerr << "Buffer overrun detected! @Streamtime: " << streamTime << std::endl;
    }

    audioBuffers->frameCounter += hwFrameCount;
    audioBuffers->timeStamp = streamTime;
    streamer->callbackFinished();

    return 0;
}
