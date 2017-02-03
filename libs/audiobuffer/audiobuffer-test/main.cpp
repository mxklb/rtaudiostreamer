#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "audiobuffer.h"

TEST_CASE( "RingBuffer", "[AudioBuffer]" )
{
    AudioBuffer* buffer = new AudioBuffer();
    SECTION("Allocation") {
        buffer->allocateRingbuffers(1024);
        REQUIRE(buffer->numberOfChannels() == 0);
        REQUIRE(buffer->ringBufferContainer.size() == 0);
        REQUIRE(buffer->ringBufferSize == 1024);
    }
    SECTION("Channels") {
        QList<unsigned int> channels({1,5});
        buffer->allocateRingbuffers(1024, channels);
        REQUIRE(buffer->numberOfChannels() == channels.size());
        REQUIRE(buffer->ringBufferContainer.size() == channels.size());

        REQUIRE(buffer->activeChannelId(1) == channels.at(1));
        REQUIRE(buffer->activeChannelId(2) == 2);
    }
    SECTION("Rotation") {
        QList<unsigned int> channels({0});
        buffer->allocateRingbuffers(10, channels);

        // Initialize buffer with values
        for( unsigned int ch=0; ch<buffer->numberOfChannels(); ch++ ) {
            QVector<double> *frames = &buffer->ringBufferContainer[ch];
            for( unsigned int i=0; i<buffer->ringBufferSize; i++ ) {
                (*frames)[i] = i;
            }
        }

        // Test buffer initialization
        foreach (QVector<double> frames, buffer->ringBufferContainer) {
            //for( int i=0; i<frames.size(); i++ ) { std::cerr << frames.at(i) << ", "; }
            REQUIRE(frames.at(3) == 3);
        }

        // Rotate and test frames
        unsigned int delta = 5;
        REQUIRE(buffer->rotateRingbuffers(delta) == true );
        foreach (QVector<double> frames, buffer->ringBufferContainer) {
            //for( int i=0; i<frames.size(); i++ ) { std::cerr << frames.at(i) << ", "; }
            REQUIRE(frames.at(delta) == 0);
        }
        REQUIRE(buffer->rotateRingbuffers(buffer->ringBufferSize+1) == false);
    }

    delete buffer;
}
