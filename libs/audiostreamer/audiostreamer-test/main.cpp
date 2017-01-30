#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "audiostreamer.h"

TEST_CASE( "AudioStreamer", "[AudioStreamer]" ) {
    SECTION("Initialization") {
        AudioStreamer streamer;
        unsigned int deviceId = 0;

        // Test numberOfInputChannels
        streamer.setActiveDevice(deviceId);
        unsigned int numOfInputs = streamer.getListOfDevices().at(deviceId).inputChannels;
        REQUIRE(streamer.numberOfInputChannels() == numOfInputs);

        // Test setActiveChannels (all channels 0..N)
        QList<unsigned int> channels;
        for( unsigned int ch=0; ch<numOfInputs; ch++ ) {
            channels.push_back(ch);
        }
        streamer.setActiveChannels(channels);
        REQUIRE(streamer.getInputChannelIds().size() == numOfInputs);
        for( unsigned int ch=0; ch<numOfInputs; ch++ ) {
            REQUIRE(streamer.getInputChannelIds().at(ch) == channels.at(ch));
        }
    }
}
