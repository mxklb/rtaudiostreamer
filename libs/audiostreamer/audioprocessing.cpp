#include <QSettings>
#include <iostream>
#include <iomanip>
#include <math.h>

#include "audioprocessing.h"

using namespace std;

// Minimal allowed sample amplitude
double AudioProcessing::silenceGate = 7.15256e-07;

/*
 * Constructor (set gate threshold from config)
 */
AudioProcessing::AudioProcessing(QObject *parent) : QObject(parent)
{
    QSettings settings;
    silenceGate = settings.value("audiostreamer/silenceGate", 7.15256e-07).toDouble();
}

/*
 * Updates the internally used audioBuffers: Using data from the given buffer.
 */
void AudioProcessing::slotUpdateRingBuffer(AudioBuffer *buffer)
{
    if( audioBuffer.rawBufferSize() != buffer->rawBufferSize() ||
        audioBuffer.ringBufferSize() != buffer->ringBufferSize() ||
        audioBuffer.numberOfChannels() != buffer->numberOfChannels() )
    {
        audioBuffer.allocate(buffer->ringBufferSize(), buffer->ringBuffer.channelIds, buffer->rawBufferSize());
    }

    audioBuffer.frameCounter = buffer->frameCounter;
    audioBuffer.streamTimeStamp = buffer->streamTimeStamp;
    audioBuffer.ringBuffer.insert(&buffer->rawBuffer->frames, buffer->rawBufferSize(), buffer->numberOfChannels(true));
}

/*
 * Perform audio analysis on the given buffer.
 */
void AudioProcessing::slotAudioProcessing()
{
    // todo: Here should be dynamic channel processing pipline ..
    printAmplitudes( absoluteAmplitudes(&audioBuffer, streamSettings.formatLimit()) );
}

/*
 * Prints the given amplitudes to the std out using carrige return.
 *
 * Additionally always prints some streaming parameters. Expected output:
 *
 *   time-stamp [frame number / buffer number] | channel i: amplitude [log10(amplitude)] | channel 1+1: ..
 */
void AudioProcessing::printAmplitudes(QList<double> amplitudes)
{
    QList<double> loudness = logAmplitudes(amplitudes);

    // Just print some results to the terminal ..
    //cerr << "\r";
    cerr << setprecision(2) << audioBuffer.streamTimeStamp << " [frame "
         << audioBuffer.frameCounter << " / buffer " << audioBuffer.frameCounter / audioBuffer.ringBufferSize() << "] ";
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
QList<double> AudioProcessing::absoluteAmplitudes(AudioBuffer *buffer, double normalize)
{
    int numOfChannels = buffer->numberOfChannels();

    QList<double> amplitudes;
    for( int ch=0; ch<numOfChannels; ch++ ) {
        amplitudes.push_back(0.0);
    }

    RingBuffer *ringBuffer = &buffer->ringBuffer;
    for( int ch=0; ch<numOfChannels; ch++ ) {
        amplitudes[ch] += accumulate(ringBuffer->bufferContainer.at(ch), normalize);
    }

    for( int ch=0; ch<numOfChannels; ch++ ) {
        amplitudes[ch] /= (double)ringBuffer->ringBufferSize;
    }

    return amplitudes;
}

/*
 * Accumulates all absolute values inside the frames vector. Optionally normalized and/or signed.
 */
double AudioProcessing::accumulate(QVector<double> frames, double norm, bool absolute)
{
    double amplitude = 0;
    foreach (double value, frames) {
        if( !absolute ) amplitude += value / norm;
        else amplitude += fabs(value / norm);
    }
    return amplitude;
}

/*
 * Converts absolute real amplitudes (range A=0..1) to relative/log loudness [dB]: factor*log10(A)
 */
QList<double> AudioProcessing::logAmplitudes(QList<double> amplitudes, double factor)
{
    QList<double> loudness;
    foreach (double amplitude, amplitudes) {
        if( amplitude < silenceGate )
            amplitude = silenceGate;
        loudness.push_back(factor*log10(amplitude));
    }
    return loudness;
}
