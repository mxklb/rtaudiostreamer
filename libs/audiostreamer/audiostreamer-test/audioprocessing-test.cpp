#ifndef AUDIOPROCESSINGTEST_CPP
#define AUDIOPROCESSINGTEST_CPP

#include "catch.hpp"
#include "audiobuffer.h"
#include "audioprocessing.h"

TEST_CASE( "AudioProcessing", "[Algorithms]" )
{
    double amplitudeValue = 0.5;
    unsigned int numberOfFrames = 10;
    double defaultValue = 32768 * amplitudeValue;

    QVector<unsigned int> channels;
    channels.push_back(0);
    channels.push_back(1);

    AudioBuffer* buffer = new AudioBuffer();
    buffer->allocate(numberOfFrames, channels, 256, defaultValue);

    QList<double> amplitudes = AudioProcessing::absoluteAmplitudes(buffer, -defaultValue/amplitudeValue);
    QList<double> loudness = AudioProcessing::logAmplitudes(amplitudes);

    SECTION("Amplitude Accumulation")
    {
        foreach (double amp, amplitudes) {
           REQUIRE(amp == amplitudeValue);
        }
        for(unsigned int ch=0; ch<buffer->numberOfChannels(); ch++) {
            REQUIRE(AudioProcessing::accumulate(buffer->ringBuffer.bufferContainer.at(ch), defaultValue) == numberOfFrames);
            REQUIRE(AudioProcessing::accumulate(buffer->ringBuffer.bufferContainer.at(ch), -defaultValue, false) == -(double)numberOfFrames);
        }
    }

    SECTION("Loudness Calculation") // L=10*log10(A)
    {
        foreach (double loud, loudness) {
           REQUIRE((float)loud == (float)-3.0102999566);
        }

        // Test 0.0, 1.0 & 0.25 amplitude
        amplitudes[0] = 0.0;
        amplitudes[1] = 1.0;
        amplitudes.push_back(0.25);
        loudness = AudioProcessing::logAmplitudes(amplitudes);
        REQUIRE((float)loudness[0] == (float)-61.4553849038);
        REQUIRE((float)loudness[1] == (float)0.0);
        REQUIRE((float)loudness[2] == (float)-6.0206);
    }

    SECTION("Processing Pipeline")
    {
        AudioProcessing* processing = new AudioProcessing(NULL);
        processing->slotUpdateRingBuffer(buffer);
        foreach (double amp, processing->absoluteAmplitudes(buffer, defaultValue/amplitudeValue)) {
           REQUIRE(amp == amplitudeValue);
        }
        processing->slotAudioProcessing();
        delete processing;
    }

    delete buffer;
}

#endif // AUDIOPROCESSINGTEST_CPP
