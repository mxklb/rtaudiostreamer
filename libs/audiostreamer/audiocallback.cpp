#include <math.h>
#include "audiobuffer.h"
#include "audiocallback.h"

/*
 * Callback triggered from running RtAudio stream.
 *
 * Copies the inputBuffer into audioStreamer's raw AudioBuffer storrage (audioBuffer->rawAudioBuffer).
 */
int AudioCallback::interleaved( void *outputBuffer, void *inputBuffer, unsigned int hwFrameCount,
                                double streamTime, RtAudioStreamStatus status, void *audioStreamer )
{
    Q_UNUSED(outputBuffer)

    if( status ) std::cerr << "Stream over/underflow detected." << std::endl;
    if( hwFrameCount == 0 ) std::cerr << "Zero frames detected." << std::endl;

    signed short *samples = (signed short*)inputBuffer;
    AudioStreamer *streamer = (AudioStreamer*)audioStreamer;
    AudioBuffer *audioBuffer = streamer->getAudioBuffer();
    unsigned int rawChannels = audioBuffer->numberOfChannels(true);

    if( rawChannels == 0 ) {
        std::cerr << "Zero channels detected." << std::endl;
        return 1;
    }

    if( !audioBuffer->rawBuffer.pushRawDataToQueue(samples, rawChannels*hwFrameCount) ) {
        std::cerr << "Buffer overrun detected! @Streamtime: " << streamTime << std::endl;
    }

    audioBuffer->frameCounter += hwFrameCount;
    audioBuffer->streamTimeStamp = streamTime;
    streamer->callbackFinished();

    return 0;
}
