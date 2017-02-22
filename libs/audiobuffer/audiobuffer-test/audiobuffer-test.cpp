#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "audiobuffer.h"

// Initialize vector of interleaved frames from 0..hwBufferSize for each channel.
template <typename type>
type* initFrames(unsigned int numOfChannels, unsigned int hwBufferSize)
{
    type* frames = new type[numOfChannels*hwBufferSize];
    for( unsigned int ch=0; ch<numOfChannels; ch++ ) {
        for( unsigned int i=0; i<hwBufferSize; i++ ) {
            frames[i*ch + ch] = i;
        }
    }
    return frames;
}

// Initializes a void* buffer of given format and size
void* initTestBuffer(unsigned int format, unsigned int numOfChannels, unsigned int hwBufferSize)
{
    void* frames = NULL;
    switch( format ) {
        case 0x1: { qint8 *f1 = initFrames<qint8>(numOfChannels,hwBufferSize); frames = (void*)f1; break; }
        case 0x2: { qint16 *f1 = initFrames<qint16>(numOfChannels,hwBufferSize); frames = (void*)f1; break; }
        case 0x8: { qint32 *f1 = initFrames<qint32>(numOfChannels,hwBufferSize); frames = (void*)f1; break; }
        case 0x10: { float *f1 = initFrames<float>(numOfChannels,hwBufferSize); frames = (void*)f1; break; }
        case 0x20: { double *f1 = initFrames<double>(numOfChannels,hwBufferSize); frames = (void*)f1; break; }
        default: { qint16 *f1 = initFrames<qint16>(numOfChannels,hwBufferSize); frames = (void*)f1; break; }
    }
    return frames;
}

// Returns true when casted void pointer (frames) contains same values than first N of buffers raw frames
bool testChannelInsertion(AudioBuffer* buffer, unsigned int format, void* frames, unsigned int numOfRawChannels, unsigned int N = 2)
{
    bool success = true;
    for( unsigned int i=0; i<numOfRawChannels*N; i++ ) {
        switch( format ) {
            case 0x1: { success = success && buffer->rawBuffer->frames[i] == ((qint8*)frames)[i]; break; }
            case 0x2: { success = success && buffer->rawBuffer->frames[i] == ((qint16*)frames)[i]; break; }
            case 0x8: { success = success && buffer->rawBuffer->frames[i] == ((qint32*)frames)[i]; break; }
            case 0x10: { success = success && buffer->rawBuffer->frames[i] == ((float*)frames)[i]; break; }
            case 0x20: { success = success && buffer->rawBuffer->frames[i] == ((double*)frames)[i]; break; }
            default: { success = success && buffer->rawBuffer->frames[i] == ((qint16*)frames)[i]; break; }
        }
    }
    return success;
}

TEST_CASE( "AudioBuffer", "[AudioBuffer]" )
{
    AudioBuffer* buffer = new AudioBuffer();

    SECTION("Allocation")
    {
        CHECK_NOFAIL(buffer->allocate(1024));
        REQUIRE(buffer->numberOfChannels() == 0);
        REQUIRE(buffer->numberOfChannels(true) == 0);
        REQUIRE(buffer->ringBuffer.bufferContainer.size() == 0);
        REQUIRE(buffer->ringBufferSize() == 1024);
    }

    SECTION("Ring Buffer -> Channels")
    {
        QVector<unsigned int> channels({1,5});
        REQUIRE(buffer->allocate(1024, channels));
        // Test ringbuffer channel setup
        RingBuffer *ringBuffer = &buffer->ringBuffer;
        REQUIRE(buffer->numberOfChannels() == channels.size());
        REQUIRE(ringBuffer->bufferContainer.size() == channels.size());
        REQUIRE(ringBuffer->channelIds.at(0) == channels.at(0));
        REQUIRE(ringBuffer->channelIds.at(1) == 5);
    }

    SECTION("Ring Buffer -> Rotation")
    {
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
            REQUIRE(frames.at(3) == 3);
        }

        // Rotate and test frames
        unsigned int delta = 5;
        REQUIRE(buffer->ringBuffer.rotateRingbuffers(delta) == true );
        foreach (QVector<double> frames, buffer->ringBuffer.bufferContainer) {
            REQUIRE(frames.at(delta) == 0);
        }
        REQUIRE_FALSE(buffer->ringBuffer.rotateRingbuffers(buffer->ringBufferSize()+1));
    }

    SECTION("Ring Buffer -> Inserting")
    {
        QVector<unsigned int> channels({1,5});
        REQUIRE(buffer->allocate(1024, channels));
        REQUIRE(buffer->ringBuffer.insert(&buffer->rawBuffer->frames, buffer->rawBufferSize(), buffer->numberOfChannels(true)));
        REQUIRE_FALSE(buffer->ringBuffer.insert(&buffer->rawBuffer->frames, buffer->ringBufferSize()*2, buffer->numberOfChannels(true)));
    }

    SECTION("Raw Buffer")
    {
        unsigned int hwBufferSize = 128;
        QVector<unsigned int> channels({1,3,5});
        REQUIRE(buffer->allocate(1024, channels, hwBufferSize));
        unsigned int numOfRawChannels = buffer->numberOfChannels(true);
        unsigned int numOfFrames = hwBufferSize*numOfRawChannels;

        // Test raw buffer channel setup
        REQUIRE(buffer->rawBufferSize() == hwBufferSize);
        REQUIRE(numOfRawChannels == channels.last() - channels.first() + 1);
        REQUIRE(buffer->rawBuffer->frames.size() == numOfFrames);

        // Testing all supported audio formats + unknown 0x33
        QVector<unsigned int> formats({0x1, 0x2, 0x8, 0x10, 0x20, 0x33});
        foreach (unsigned int format, formats)
        {
            void* frames = initTestBuffer(format, numOfRawChannels, hwBufferSize);
            REQUIRE(buffer->prepareRawBuffer(format, hwBufferSize));

            // Test raw buffer insert operation
            REQUIRE(buffer->rawBuffer->insert(frames, numOfFrames, true));
            REQUIRE(testChannelInsertion(buffer, format, frames, numOfRawChannels));

            // Check reducing frame count to push (once)
            if( format == formats.first() )
                REQUIRE(buffer->rawBuffer->insert(frames, numOfFrames*2));
        }
    }
    delete buffer;
}
