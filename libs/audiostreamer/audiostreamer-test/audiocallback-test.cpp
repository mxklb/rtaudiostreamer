#include <RtAudio.h>
#include "catch.hpp"
#include "audiobuffer.h"
#include "audiocallback.h"
#include "audiostreamer.h"

AudioBuffer initBuffers(unsigned int hwBufferSize, unsigned int audioBufferSize, AudioStreamer* streamer, signed short* hwBuffer)
{
    AudioBuffer buffer;
    unsigned int numOfFrames = hwBufferSize;
    QList<unsigned int> channels = streamer->getInputChannelIds();
    buffer.allocateRingbuffers(audioBufferSize, channels);

    // Init interleaved inputBuffer. Each channel values [0..numOfFrames]
    unsigned int index = 0;
    for( unsigned int i=0; i<numOfFrames; i++) {
        foreach (unsigned int ch, channels) {
            hwBuffer[index++] = i;
        }
    }
    return buffer;
}

TEST_CASE( "AudioCallback", "[RtAudio]" ) {
    AudioStreamer streamer;
    if( !streamer.getListOfDevices().isEmpty() ) {
        unsigned int bufferSize = 1024;
        unsigned int numOfFrames = 256;
        unsigned int numOfChannels = streamer.numberOfInputChannels();

        SECTION("Correct Buffer Size") {
            signed short* hwBuffer = new signed short[numOfFrames*numOfChannels];
            AudioBuffer buffer = initBuffers(numOfFrames, bufferSize, &streamer, hwBuffer);

            REQUIRE(buffer.frameCounter == 0);
            REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, numOfFrames, 1., 0, &buffer) == 0 );

            // Check frame counter
            REQUIRE(buffer.frameCounter == numOfFrames);

            // Check if buffer is correctly filled
            for( unsigned int ch=0; ch<numOfChannels; ch++) {
                REQUIRE(buffer.ringBufferContainer.at(ch)[2] == 2);
            }
        }

        SECTION("Buffer Too Small") {
            bufferSize = 128;
            signed short* hwBuffer = new signed short[numOfFrames*numOfChannels];
            AudioBuffer buffer = initBuffers(numOfFrames, bufferSize, &streamer, hwBuffer);

            REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, numOfFrames, 1., 0, &buffer) == 0 );

            // Check frame counter
            REQUIRE(buffer.frameCounter == bufferSize);

            // Check if buffer is correctly filled
            for( unsigned int ch=0; ch<numOfChannels; ch++) {
                REQUIRE(buffer.ringBufferContainer.at(ch)[2] == 2);
            }
        }

        SECTION("Zero Channel Count") {
            signed short* hwBuffer = new signed short[numOfFrames*numOfChannels];
            AudioBuffer buffer = initBuffers(numOfFrames, bufferSize, &streamer, hwBuffer);
            buffer.allocateRingbuffers(bufferSize);
            REQUIRE(buffer.ringBufferContainer.isEmpty());
            REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, numOfFrames, 1., 0, &buffer) == 0 );
            REQUIRE(buffer.frameCounter == 0);
        }
    } else WARN("Tests disabled: RtAudio no audio device found!");
}
