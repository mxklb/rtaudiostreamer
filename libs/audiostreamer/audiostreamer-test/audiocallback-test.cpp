#include <RtAudio.h>
#include "catch.hpp"
#include "audiobuffer.h"
#include "audiocallback.h"
#include "audiostreamer.h"

TEST_CASE( "AudioCallback", "[RtAudio]" ) {
    AudioBuffer buffer;
    AudioStreamer streamer;
    if( streamer.getListOfDevices().size() > 0 ) {
        unsigned int numOfFrames = 256;
        QList<unsigned int> channels = streamer.getInputChannelIds();
        unsigned int numOfChannels = streamer.numberOfInputChannels();
        buffer.allocateRingbuffers(1024, channels);
        signed short* inputBuffer = new signed short[numOfFrames*numOfChannels];

        // Init interleaved inputBuffer. Each channel values [0..numOfFrames]
        unsigned int index = 0;
        for( unsigned int i=0; i<numOfFrames; i++) {
            foreach (unsigned int ch, channels) {
                inputBuffer[index++] = i;
            }
        }

        REQUIRE(buffer.frameCounter == 0);
        REQUIRE(audioStreamCallback(NULL, inputBuffer, numOfFrames, 1., 0, &buffer) == 0 );

        // Check frame counter
        REQUIRE(buffer.frameCounter == numOfFrames);

        // Check if buffer is correctly filled
        for( unsigned int ch=0; ch<numOfChannels; ch++) {
            REQUIRE(buffer.ringBufferContainer.at(ch)[2] == 2);
        }
    } else WARN("Tests disabled: RtAudio didn't find any channels!");
}
