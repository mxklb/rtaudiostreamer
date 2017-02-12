#include <iostream>
#include <iomanip>

#include "audioprocessing.h"

using namespace std;

AudioProcessing::AudioProcessing(QObject *parent) : QObject(parent) { }

/*
 * Updates the internally used audioBuffers: Using data from the given buffer.
 */
void AudioProcessing::slotUpdateRingBuffer(AudioBuffer *buffer)
{
    if( audioBuffer.ringBufferSize() != buffer->ringBufferSize()  ||
        audioBuffer.numberOfChannels() != buffer->numberOfChannels() )
    {
        audioBuffer.allocate(buffer->ringBufferSize(), buffer->ringBuffer.channelIds, buffer->rawBufferSize());
    }

    audioBuffer.frameCounter = buffer->frameCounter;
    audioBuffer.streamTimeStamp = buffer->streamTimeStamp;
    audioBuffer.ringBuffer.insert(&buffer->rawBuffer.rawFrames, buffer->rawBufferSize(), buffer->numberOfChannels(true));
}

/*
 * Perform audio analysis on the given buffer.
 */
void AudioProcessing::slotAudioProcessing()
{
    // todo: Here should be dynamic channel processing pipline ..

    QList<double> amplitudes = absoluteAmplitudes(&audioBuffer);
    QList<double> loudness = logLoudness(amplitudes);

    // Just print some results to the terminal ..
    cerr << setprecision(2) << audioBuffer.streamTimeStamp << " [frame "
         << audioBuffer.frameCounter << " / buffer " << audioBuffer.frameCounter / audioBuffer.ringBufferSize() << "]\t";
    for( unsigned int ch=0; ch<audioBuffer.numberOfChannels(); ch++ ) {
        unsigned int channel = audioBuffer.ringBuffer.channelIds.at(ch);
        cerr << " | Ch" << channel << ": " << setiosflags(ios::fixed) << setprecision(7)
                  << amplitudes[ch] << " [" << setprecision(1) << loudness[ch] << " dB]";
    }
    cerr << "\r";
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

    RingBuffer *ringBuffer = &buffer->ringBuffer;
    for( int ch=0; ch<numOfChannels; ch++ ) {
        for( unsigned int i=0; i<ringBuffer->ringBufferSize; i++ ) {
            double amplitude = ringBuffer->bufferContainer.at(ch)[i];
            amplitudes[ch] += fabs(amplitude / 32768.); // todo: get factor from used rtaudio data type
        }
    }

    for( int ch=0; ch<numOfChannels; ch++ ) {
        amplitudes[ch] /= (double)ringBuffer->ringBufferSize;
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
