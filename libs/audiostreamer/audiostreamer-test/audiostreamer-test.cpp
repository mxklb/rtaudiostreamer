#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "audiostreamer.h"
#include <unistd.h>

class AudioStreamerMoc : public AudioStreamer {
public:
    void updateProcessingBuffer() { slotUpdateBuffers(); }
    AudioBuffer* getGrabbingBuffer() { return getAudioBuffer(); }
};

TEST_CASE( "AudioStreamer", "[RtAudio]" )
{
    AudioStreamerMoc* streamer = new AudioStreamerMoc();

    SECTION("Initialization")
    {
        streamer->printListOfDevices();
        if( !streamer->getListOfDevices().isEmpty() ) {
            // Test numberOfInputChannels
            unsigned int deviceId = 0;
            streamer->setActiveDevice(deviceId);
            unsigned int numOfInputChannels = streamer->getListOfDevices().at(deviceId).inputChannels;
            REQUIRE(streamer->numberOfInputChannels() == numOfInputChannels);

            // Test setActiveChannels (all channels 0..N-1 and reverse N-1..0)
            QList<bool> channelSort({false, true});
            foreach (bool reverse, channelSort) {
                QVector<unsigned int> channels;
                for( unsigned int ch=0; ch<numOfInputChannels; ch++ ) {
                    if( !reverse ) channels.push_back(ch);
                    else channels.push_front(ch);
                }
                streamer->setActiveChannels(channels);
                REQUIRE(streamer->getInputChannelIds().size() == numOfInputChannels);

                for( unsigned int ch=0; ch<numOfInputChannels; ch++ ) {
                    if( !reverse ) REQUIRE(streamer->getInputChannelIds().at(ch) == channels.at(ch));
                    else REQUIRE(streamer->getInputChannelIds().at(ch) == channels.at(numOfInputChannels-1-ch));
                }
            }
        } else WARN("Testing disabled: RtAudio no audio device found!");
    }

    SECTION("Stream Settings")
    {
        REQUIRE(StreamSettings::formatLimit(RTAUDIO_SINT8) == 127);
        REQUIRE(StreamSettings::formatLimit(RTAUDIO_SINT16) == 32767);
        REQUIRE(StreamSettings::formatLimit(RTAUDIO_SINT32) == 2147483647);
        REQUIRE(StreamSettings::formatLimit(RTAUDIO_FLOAT32) == std::numeric_limits<float>::max());
        REQUIRE(StreamSettings::formatLimit(RTAUDIO_FLOAT64) == std::numeric_limits<double>::max());
        REQUIRE(StreamSettings::formatLimit(0x40) == 32767);
    }

    SECTION("Streaming")
    {
        REQUIRE(streamer->getGrabbingBuffer() != NULL);

        if( !streamer->getListOfDevices().isEmpty() ) {
            // Start streaming
            if( streamer->startStream() ) {
                usleep(200);
                REQUIRE(streamer->stopStream() == true);
            } else {
                WARN("Streaming failed: RtAudio can't start stream!");
            }

            streamer->updateProcessingBuffer();

            // Test zero channels
            QVector<unsigned int> channels;
            streamer->setActiveChannels(channels);
            REQUIRE_FALSE( streamer->startStream() );
        } else WARN("Testing disabled: RtAudio no audio device found!");
    }

    if( streamer ) delete streamer;
}
