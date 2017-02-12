#include <RtAudio.h>
#include "catch.hpp"
#include "audiobuffer.h"
#include "audiocallback.h"
#include "audiostreamer.h"

/*
 * Mocup to access protected members
 */
class testAudioStreamer : public AudioStreamer
{
public:
    AudioBuffer* getStreamerBuffer() {
        return getAudioBuffer();
    }
};

/*
 * Init interleaved hwBuffer. Each channel values [0..hwBufferSize]
 */
void initHwBuffer(signed short* buffer, unsigned int hwBufferSize, unsigned int numOfChannels)
{
    unsigned int index = 0;
    for( unsigned int i=0; i<hwBufferSize; i++) {
        for( unsigned int ch=0; ch<numOfChannels; ch++) {
            Q_UNUSED(ch)
            buffer[index++] = i;
        }
    }
}

/*
 * Prints first range values of each channel to the console
 */
void printBuffer(QList<QVector<double> > bufferContainer, int range = 10) {
    int ch = 0;
    foreach (QVector<double> buffer, bufferContainer) {
        std::cerr << "ch" << ch++;
        for(int i=0; i<range; i++ ) {
            std::cerr << ", " << buffer.at(i);
        }
        std::cerr << std::endl;
    }
}


TEST_CASE( "AudioCallback", "[RtAudio]" ) {
    testAudioStreamer streamer;
    if( !streamer.getListOfDevices().isEmpty() ) {
        unsigned int ringBufferSize = 1024;
        unsigned int hwBufferSize = 256;
        unsigned int numOfRawChannels = 0;
        QVector<unsigned int> channelIds = streamer.getInputChannelIds();
        unsigned int numOfChannels = channelIds.size();
        AudioBuffer *buffer = streamer.getStreamerBuffer();
        signed short* hwBuffer = NULL;

        if( channelIds.size() > 0 )
        {
            numOfRawChannels = channelIds.last() - channelIds.first() + 1;

            hwBuffer = new signed short[hwBufferSize*numOfRawChannels];
            initHwBuffer(hwBuffer, hwBufferSize, numOfRawChannels);

            SECTION("With buffer size > hw buffer") {
                REQUIRE(buffer->allocate(ringBufferSize, channelIds, hwBufferSize));

                REQUIRE(buffer->frameCounter == 0);
                REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, hwBufferSize, 1., 0, &streamer) == 0 );
                REQUIRE(buffer->frameCounter == hwBufferSize);

                // Dequeue raw frames and populate ringbuffers
                REQUIRE(buffer->rawBuffer.popRawDataFromQueue());
                REQUIRE(buffer->ringBuffer.insert(&buffer->rawBuffer.rawFrames, hwBufferSize, numOfRawChannels));

                // Check if buffer is correctly filled
                for( unsigned int ch=0; ch<numOfChannels; ch++) {
                    REQUIRE(buffer->ringBuffer.bufferContainer.at(ch)[2] == 2);
                }
            }

            SECTION("With buffer size < hw buffer") {
                ringBufferSize = 128;
                REQUIRE(buffer->allocate(ringBufferSize, channelIds, hwBufferSize));

                REQUIRE(buffer->frameCounter == 0);
                REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, hwBufferSize, 1., 0, &streamer) == 0 );
                REQUIRE(buffer->frameCounter == hwBufferSize);

                // Dequeue raw frames and populate ringbuffers
                REQUIRE(buffer->rawBuffer.popRawDataFromQueue());
                REQUIRE(buffer->ringBuffer.insert(&buffer->rawBuffer.rawFrames, ringBufferSize, numOfRawChannels));

                // Check if buffer is correctly filled
                for( unsigned int ch=0; ch<numOfChannels; ch++) {
                    REQUIRE(buffer->ringBuffer.bufferContainer.at(ch)[2] == 2);
                }
            }

            SECTION("Without channels") {
                REQUIRE_FALSE(buffer->allocate(ringBufferSize));
                REQUIRE(buffer->rawBuffer.rawFrames.isEmpty());
                REQUIRE(buffer->ringBuffer.channelIds.isEmpty());
                REQUIRE(buffer->ringBuffer.bufferContainer.isEmpty());
                REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, hwBufferSize, 1., 0, &streamer) == 1 );
                REQUIRE(buffer->frameCounter == 0);
            }
        }
        delete[] hwBuffer;
    } else WARN("Tests disabled: RtAudio no audio device found!");
}
