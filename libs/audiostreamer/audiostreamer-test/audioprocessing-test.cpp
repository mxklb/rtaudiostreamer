#ifndef AUDIOPROCESSINGTEST_CPP
#define AUDIOPROCESSINGTEST_CPP

#include "catch.hpp"
#include "audiobuffer.h"
#include "audioprocessing.h"

TEST_CASE( "AudioProcessing", "[Algorithms]" ) {
    AudioBuffer* buffer = new AudioBuffer();
    QList<unsigned int> channels;
    channels.push_back(0);
    channels.push_back(1);
    buffer->allocateRingbuffers(100, channels, -32768/2);

    QList<double> amplitudes = AudioProcessing::absoluteAmplitudes(buffer);
    QList<double> loudness = AudioProcessing::logLoudness(amplitudes);

    SECTION("Amplitude calculation") {
        foreach (double amp, amplitudes) {
           REQUIRE(amp == 0.5);
        }
    }

    SECTION("Loudness calculation") { // L=10*log10(A)
        foreach (double loud, loudness) {
           REQUIRE((float)loud == (float)-3.0102999566);
        }

        // Test 0.0 & 1.0 amplitude
        amplitudes[0] = 0.0;
        amplitudes[1] = 1.0;
        amplitudes.push_back(0.25);
        loudness = AudioProcessing::logLoudness(amplitudes);
        REQUIRE((float)loudness[0] == (float)-61.4553849038);
        REQUIRE((float)loudness[1] == (float)0.0);
        REQUIRE((float)loudness[2] == (float)-6.0206);
    }

    SECTION("Full processing pipeline") {
        AudioProcessing* processing = new AudioProcessing(NULL);
        processing->processAudio(buffer);
        delete processing;
    }

    delete buffer;
}

#endif // AUDIOPROCESSINGTEST_CPP
