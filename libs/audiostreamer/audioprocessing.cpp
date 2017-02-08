#include <iostream>
#include <iomanip>
#include <math.h>

#include "audioprocessing.h"

using namespace std;

AudioProcessing::AudioProcessing(QObject *parent) : QObject(parent)
{
}

void AudioProcessing::processRawAudio(QVector<signed int> *rawData, unsigned int numOfChannels)
{
    updateProcessingBuffer(rawData, numOfChannels);
    processAudio(&audioBuffers);
}


void AudioProcessing::updateProcessingBuffer(QVector<signed int> *rawData, unsigned int numOfChannels)
{
    unsigned int lowestChannelId = audioBuffers.activeChannelId(0);
    unsigned int hwBufferSize = rawData->size() / numOfChannels; // !!! stimmt nicht immer !!!
    unsigned int numberOfFrames = hwBufferSize;

    if( !audioBuffers.rotateRingbuffers(hwBufferSize) ) {
        numberOfFrames = audioBuffers.ringBufferSize;
    }

    for( unsigned int ch=0; ch<numOfChannels; ch++ ) {
        unsigned int channelId = audioBuffers.activeChannelId(ch) - lowestChannelId;
        QVector<double> *buffer = &audioBuffers.ringBufferContainer[ch];
        for( unsigned int i=0; i<numberOfFrames; i++ ) {
            (*buffer)[i] = (double)rawData->at(i * numOfChannels + channelId);
        }
    }
}

/*
 * Perform audio analysis on the given buffer.
 */
void AudioProcessing::processAudio(AudioBuffer *buffer)
{
    // todo: Here should be dynamic channel processing pipline ..

    QList<double> amplitudes = absoluteAmplitudes(buffer);
    QList<double> loudness = logLoudness(amplitudes);

    // Just print some results to the terminal ..
    std::cerr << "[buffer " << buffer->frameCounter / buffer->ringBufferSize << "]\t";
    for( unsigned int ch=0; ch<buffer->numberOfChannels(); ch++ ) {
        unsigned int channel = buffer->activeChannelId(ch);
        std::cerr << " | Ch" << channel << ": " << std::setiosflags(std::ios::fixed) << std::setprecision(7)
                  << amplitudes[ch] << " [" << std::setprecision(1) << loudness[ch] << " dB]";
    }
    std::cerr << endl;// "\r";
}

/*
 * Calculates the absolute mean amplitude for each channel.
 * Note: Returns absolute values in range 0..1
 */
QList<double> AudioProcessing::absoluteAmplitudes(AudioBuffer *buffer)
{
    int numOfChannels = buffer->numberOfChannels();

    QList<double> amplitudes;
    for( int ch=0; ch<numOfChannels; ch++ ) {
        amplitudes.push_back(0.0);
    }

    for( int ch=0; ch<numOfChannels; ch++ ) {
        for( unsigned int i=0; i<buffer->ringBufferSize; i++ ) {
            double amplitude = buffer->ringBufferContainer.at(ch)[i];
            amplitudes[ch] += fabs(amplitude / 32768.); // todo: get factor from used rtaudio data type
        }
    }

    for( int ch=0; ch<numOfChannels; ch++ ) {
        amplitudes[ch] /= (double)buffer->ringBufferSize;
    }

    return amplitudes;
}

/*
 * Converts absolute real amplitudes (range A=0..1) to relative/log loudness [dB]: 10*log10(A)
 */
QList<double> AudioProcessing::logLoudness(QList<double> amplitudes)
{
    QList<double> loudness;
    foreach (double amplitude, amplitudes) {
        if( amplitude < 7.15256e-07 )
            amplitude = 7.15256e-07; // gateing ?
        loudness.push_back(10*log10(amplitude));
    }
    return loudness;
}
