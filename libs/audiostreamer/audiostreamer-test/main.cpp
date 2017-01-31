#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "audiostreamer.h"

TEST_CASE( "AudioStreamer", "[AudioStreamer]" ) {
    SECTION("Initialization") {
        AudioStreamer streamer;
        unsigned int deviceId = 0;

        if( streamer.getListOfDevices().size() != 0 ) {
            // Test numberOfInputChannels
            streamer.setActiveDevice(deviceId);
            unsigned int numOfInputChannels = streamer.getListOfDevices().at(deviceId).inputChannels;
            REQUIRE(streamer.numberOfInputChannels() == numOfInputChannels);

            // Test setActiveChannels (all channels 0..N-1 and reverse N-1..0)
            QList<bool> channelSort({false, true});
            foreach (bool reverse, channelSort) {
                QList<unsigned int> channels;
                for( unsigned int ch=0; ch<numOfInputChannels; ch++ ) {
                    if( !reverse ) channels.push_back(ch);
                    else channels.push_front(ch);
                }
                streamer.setActiveChannels(channels);
                REQUIRE(streamer.getInputChannelIds().size() == numOfInputChannels);

                for( unsigned int ch=0; ch<numOfInputChannels; ch++ ) {
                    if( !reverse ) REQUIRE(streamer.getInputChannelIds().at(ch) == channels.at(ch));
                    else REQUIRE(streamer.getInputChannelIds().at(ch) == channels.at(numOfInputChannels-1-ch));
                }
            }
        }
    }
}
