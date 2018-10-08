#include <math.h>
#include "audiobuffer.h"
#include "audiocallback.h"

/*
 * Callback triggered from running RtAudio stream.
 *
 * Copies the inputBuffer into audioStreamer's raw AudioBuffer storrage (audioBuffer->rawAudioBuffer).
 */
int AudioCallback::interleaved( void *outputBuffer, void *inputBuffer, unsigned int hwFrameCount,
                                double streamTime, RtAudioStreamStatus status, void *streamer )
{
    Q_UNUSED(outputBuffer)

    if( status ) std::cerr << "Stream over/underflow detected.";
    if( hwFrameCount == 0 ) std::cerr << "Zero frames detected.";

    AudioStreamer *audioStreamer = (AudioStreamer*)streamer;
    AudioBuffer *audioBuffer = audioStreamer->getAudioBuffer();
    unsigned int rawChannels = audioBuffer->numberOfChannels(true);

    if( rawChannels == 0 ) {
        std::cerr << "Zero channels detected.";
        return 1;
    }

    if( !audioBuffer->rawBuffer->insert(inputBuffer, rawChannels*hwFrameCount) ) {
        std::cerr << "Buffer overrun detected! @Streamtime: " << streamTime;
    }

    audioBuffer->frameCounter += hwFrameCount;
    audioBuffer->streamTimeStamp = streamTime;
    audioStreamer->callbackFinished();

    return 0;
}
