#include <RtAudio.h>
#include <unistd.h>
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
    AudioBuffer* getStreamerBuffer() { return getAudioBuffer(); }
    void startIntervalTimer() { processingIntervalTimer.start(); }
    void setInterval(unsigned int ms) { processingInterval = ms; }
    bool pollProcessing(){ return processLatestAudio(); }
};

/*
 * Init interleaved hwBuffer. Each channel values [0..hwBufferSize]
 */
template <typename type>
void initHwBuffer(type* buffer, unsigned int hwBufferSize, unsigned int numOfChannels)
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
void printBuffer(QList<QVector<double> > bufferContainer, int range = 10)
{
    int ch = 0;
    foreach (QVector<double> buffer, bufferContainer) {
        std::cerr << "ch" << ch++;
        for(int i=0; i<range; i++ ) {
            std::cerr << ", " << buffer.at(i);
        }
        std::cerr << std::endl;
    }
}

TEST_CASE( "AudioCallback", "[RtAudio]" )
{
    testAudioStreamer streamer;

    if( !streamer.getListOfDevices().isEmpty() )
    {
        QVector<unsigned int> channelIds = streamer.getInputChannelIds();

        if( channelIds.size() > 0 )
        {
            AudioBuffer *buffer = streamer.getStreamerBuffer();
            unsigned int numOfRawChannels = channelIds.last() - channelIds.first() + 1;
            unsigned int numOfChannels = channelIds.size();

            unsigned int hwBufferSize = 256;
            unsigned int ringBufferSize = 1024;

            // Setup frames array/vector
            qint16* hwBuffer = new qint16[hwBufferSize*numOfRawChannels];
            initHwBuffer(hwBuffer, hwBufferSize, numOfRawChannels);

            SECTION("Ringbuffer size > raw buffer size") {
                REQUIRE(buffer->allocate(ringBufferSize, channelIds, hwBufferSize));

                REQUIRE(buffer->frameCounter == 0);
                REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, hwBufferSize, 1., 0, &streamer) == 0 );
                REQUIRE(buffer->frameCounter == hwBufferSize);
                
                // Not full enough buffer
                REQUIRE_FALSE(buffer->isFilled());

                // Dequeue raw frames and populate ringbuffers
                REQUIRE(buffer->rawBuffer->grabFramesFromQueue());
                REQUIRE(buffer->ringBuffer.insert(&buffer->rawBuffer->frames, hwBufferSize, numOfRawChannels));

                // Check if buffer is correctly filled
                for( unsigned int ch=0; ch<numOfChannels; ch++) {
                    REQUIRE(buffer->ringBuffer.bufferContainer.at(ch)[2] == 2);
                }

                // Check framecounter increment and check isFilled method.
                REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, hwBufferSize, 1., 0, &streamer) == 0 );
                REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, hwBufferSize, 1., 0, &streamer) == 0 );
                REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, hwBufferSize, 1., 0, &streamer) == 0 );
                REQUIRE(buffer->isFilled());

                // Check interval process polling
                int interval = 10;
                streamer.setInterval(interval);
                streamer.startIntervalTimer();
                REQUIRE_FALSE(streamer.pollProcessing());
                usleep(interval*1200);
                REQUIRE(streamer.pollProcessing());
                REQUIRE_FALSE(streamer.pollProcessing());
                usleep(interval*250);
                REQUIRE_FALSE(streamer.pollProcessing());
                usleep(interval*1200);
                REQUIRE(streamer.pollProcessing());
            }

            SECTION("Ringbuffer size < raw buffer size") {
                ringBufferSize = 128;
                REQUIRE(buffer->allocate(ringBufferSize, channelIds, hwBufferSize));

                REQUIRE(buffer->frameCounter == 0);
                REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, hwBufferSize, 1., 0, &streamer) == 0 );
                REQUIRE(buffer->frameCounter == hwBufferSize);

                // Dequeue raw frames and populate ringbuffers
                REQUIRE(buffer->rawBuffer->grabFramesFromQueue());
                REQUIRE(buffer->ringBuffer.insert(&buffer->rawBuffer->frames, ringBufferSize, numOfRawChannels));

                // Check if buffer is correctly filled
                for( unsigned int ch=0; ch<numOfChannels; ch++) {
                    REQUIRE(buffer->ringBuffer.bufferContainer.at(ch)[2] == 2);
                }

                // Check shifting N > ringbuffer size frames
                REQUIRE_FALSE(buffer->ringBuffer.insert(&buffer->rawBuffer->frames, ringBufferSize*2, numOfRawChannels));
            }

            SECTION("Without channels") {
                ringBufferSize = 0;
                REQUIRE_FALSE(buffer->allocate(ringBufferSize));
                REQUIRE(buffer->rawBuffer->frames.isEmpty());
                REQUIRE(buffer->ringBuffer.channelIds.isEmpty());
                REQUIRE(buffer->ringBuffer.bufferContainer.isEmpty());
                REQUIRE(AudioCallback::interleaved(NULL, hwBuffer, hwBufferSize, 1., 0, &streamer) == 1 );
                REQUIRE(buffer->frameCounter == 0);
                REQUIRE_FALSE(buffer->isFilled());
            }
            if( hwBuffer ) { delete[] hwBuffer; }
        }
    } else WARN("Tests disabled: RtAudio no audio device found!");
}
