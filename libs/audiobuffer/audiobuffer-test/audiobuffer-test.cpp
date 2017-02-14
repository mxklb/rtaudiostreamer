#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "audiobuffer.h"

TEST_CASE( "AudioBuffer", "[AudioBuffer]" )
{
    AudioBuffer* buffer = new AudioBuffer();

    SECTION("Allocation") {
        CHECK_NOFAIL(buffer->allocate(1024));
        REQUIRE(buffer->numberOfChannels() == 0);
        REQUIRE(buffer->numberOfChannels(true) == 0);
        REQUIRE(buffer->ringBuffer.bufferContainer.size() == 0);
        REQUIRE(buffer->ringBufferSize() == 1024);
    }

    SECTION("Ring Buffer -> Channels") {
        QVector<unsigned int> channels({1,5});
        REQUIRE(buffer->allocate(1024, channels));
        // Test ringbuffer channel setup
        RingBuffer *ringBuffer = &buffer->ringBuffer;
        REQUIRE(buffer->numberOfChannels() == channels.size());
        REQUIRE(ringBuffer->bufferContainer.size() == channels.size());
        REQUIRE(ringBuffer->channelIds.at(0) == channels.at(0));
        REQUIRE(ringBuffer->channelIds.at(1) == 5);
    }

    SECTION("Ring Buffer -> Rotation") {
        QVector<unsigned int> channels({0});
        buffer->allocate(10, channels);

        // Initialize buffer with values
        for( unsigned int ch=0; ch<buffer->numberOfChannels(); ch++ ) {
            QVector<double> *frames = &buffer->ringBuffer.bufferContainer[ch];
            for( unsigned int i=0; i<buffer->ringBufferSize(); i++ ) {
                (*frames)[i] = i;
            }
        }

        // Test buffer initialization
        foreach (QVector<double> frames, buffer->ringBuffer.bufferContainer) {
            //for( int i=0; i<frames.size(); i++ ) { std::cerr << frames.at(i) << ", "; }
            REQUIRE(frames.at(3) == 3);
        }

        // Rotate and test frames
        unsigned int delta = 5;
        REQUIRE(buffer->ringBuffer.rotateRingbuffers(delta) == true );
        foreach (QVector<double> frames, buffer->ringBuffer.bufferContainer) {
            //for( int i=0; i<frames.size(); i++ ) { std::cerr << frames.at(i) << ", "; }
            REQUIRE(frames.at(delta) == 0);
        }
        REQUIRE(buffer->ringBuffer.rotateRingbuffers(buffer->ringBufferSize()+1) == false);
    }

    SECTION("Raw Buffer -> Channels") {
        unsigned int hwBufferSize = 128;
        QVector<unsigned int> channels({1,3,5});
        REQUIRE(buffer->allocate(1024, channels, hwBufferSize));

        // Test raw buffer channel setup
        unsigned int numOfRawChannels = buffer->numberOfChannels(true);
        REQUIRE(numOfRawChannels == channels.last() - channels.first() + 1);
        REQUIRE(buffer->rawBufferSize() == hwBufferSize);

        RawBuffer *rawBuffer = &buffer->rawBuffer;
        REQUIRE(rawBuffer->rawFrames.size() == numOfRawChannels * hwBufferSize);

        // Test raw buffer insert operation
        unsigned int numOfFrames = hwBufferSize*numOfRawChannels;
        QVector<signed short> frames = QVector<signed short>(numOfFrames, -10);
        for( unsigned int ch=0; ch<numOfRawChannels; ch++ ) {
            for( unsigned int i=0; i<hwBufferSize; i++ ) {
                frames[i*ch + ch] = i;
            }
        }

        REQUIRE(rawBuffer->insert(frames.data(), numOfFrames, true));

        // Check first two frames from each channel
        for( unsigned int i=0; i<numOfRawChannels*2; i++ ) {
            REQUIRE(rawBuffer->rawFrames[i] == frames[i] );
        }
    }
    delete buffer;
}
