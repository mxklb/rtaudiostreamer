#include "audiostreamer.h"
#include "audiocallback.h"
#include <QSettings>

using namespace std;

/*
 * Class to handle the RtAudio soundcard API.
 */
AudioStreamer::AudioStreamer(QObject *parent) : QObject(parent)
{
    QSettings settings;
    processingInterval = settings.value("audiostreamer/processingInterval", 50).toUInt();

    try {
        rtAudio = new RtAudio();
        activeDeviceId = settings.value("audiostreamer/activeDeviceId", rtAudio->getDefaultInputDevice()).toUInt();
    }
#ifdef RTERROR_H
    catch( RtError &error )
#else
    catch( RtAudioError& error )
#endif
    { error.printMessage(); }

    setupDeviceList();
    allocateRingBuffers( getInputChannelIds(activeDeviceId), settings.value("audiostreamer/processingBufferSize", 8192).toUInt(), streamSettings.hwBufferSize );

    // Use signal from RtAudioCallback to trigger buffer update in main thread.
    connect(this, SIGNAL(audioCallbackFinished()), SLOT(slotUpdateBuffers()));
}

/*
 * Class destructor stop all streams and free ..
 */
AudioStreamer::~AudioStreamer()
{
    stopStream();
    delete rtAudio;
}

/*
 * Request a list of devices known to RtAudio Api and collect them in a list.
 */
void AudioStreamer::setupDeviceList()
{
    devices.clear();

    try {
        for (unsigned int i = 0; i < rtAudio->getDeviceCount(); i++) {
            RtAudio::DeviceInfo info = rtAudio->getDeviceInfo(i);
            devices.push_back(info);
        }
    }
#ifdef RTERROR_H
    catch( RtError &error )
#else
    catch( RtAudioError &error )
#endif
    { error.printMessage(); }
}

/*
 * Returns the list of detected RtAudio devices.
 */
QList<RtAudio::DeviceInfo> AudioStreamer::getListOfDevices()
{
    return devices;
}

/*
 * Prints the list of devices to to console.
 */
void AudioStreamer::printListOfDevices()
{
    int index = 0;
    foreach (RtAudio::DeviceInfo info, devices) {
        cout << (int)info.isDefaultInput << " [" << index++ << "] " << info.name;
        cout << ": max outputs = " << info.outputChannels << " ";
        cout << ": max inputs = " << info.inputChannels << " ";
        cout << ": dup chCount = " << info.duplexChannels << "\n";
        std::vector<unsigned int> sampleRates = info.sampleRates;
        cout << "  SR [kHz]: ";
        for( unsigned int j=0; j<sampleRates.size(); j++ ) {
            cout << sampleRates[j] << ", ";
        }
        cout << endl;
    }
}

/*
 * Returns a list of ids of all available input channels of the active device.
 */
QVector<unsigned int> AudioStreamer::getInputChannelIds(int deviceId)
{
    QVector<unsigned int> channels;
    for( unsigned int i=0; i<numberOfInputChannels(deviceId); i++ ) {
        channels.push_back(i);
    }
    return channels;
}

/*
 * Returns the number of input channels for the active device (deviceId == -1).
 * If the given deviceId > -1 returns the input channels of this device.
 */
unsigned int AudioStreamer::numberOfInputChannels(int deviceId)
{
    unsigned int channelCount = 0;
    unsigned int id = activeDeviceId;
    if( deviceId > -1 ) id = deviceId;
    if( devices.size() > 0 && id < (unsigned int)devices.size() )
        channelCount = devices.at(id).inputChannels;
    return channelCount;
}

/*
 * Allocates ringbuffers for aquisition and processing of each channel for the active device.
 */
void AudioStreamer::allocateRingBuffers(QVector<unsigned int> channels, unsigned int ringBufferSize, unsigned int hwBufferSize)
{
    audioBuffer.allocate(ringBufferSize, channels, hwBufferSize);
}

/*
 * Changes the channel ids used for audio streaming.
 */
void AudioStreamer::setActiveChannels(QVector<unsigned int> channelIds)
{
    setActiveDevice(activeDeviceId, channelIds);
}

/*
 * Switches the audio device to be used. Stops running stream and resets buffers.
 */
void AudioStreamer::setActiveDevice(unsigned int id, QVector<unsigned int> channelIds)
{
    stopStream();
    activeDeviceId = id;
    //if( channels.isEmpty() ) channels = getInputChannelIds();
    allocateRingBuffers(channelIds, audioBuffer.ringBufferSize(), streamSettings.hwBufferSize);
}

/*
 * Opens and starts the audio stream to be prossessed by audioprocessor callback.
 */
bool AudioStreamer::startStream(StreamSettings settings)
{
    if( audioBuffer.numberOfChannels() == 0 ) {
        cout << "Warning: Unable to start stream without input channels!" << endl;
        return false;
    }

    streamSettings = settings;

    RtAudio::StreamParameters parameters = settings.parameters;
    parameters.deviceId = activeDeviceId;
    parameters.nChannels = audioBuffer.numberOfChannels(true);
    parameters.firstChannel = audioBuffer.ringBuffer.channelIds.first();

    double msInBuffer = 1000. * audioBuffer.ringBufferSize() / (double)settings.hwSampleRate;

    cout << "Starting streaming " << parameters.nChannels << " channels for device [" << activeDeviceId << "], buffer " << audioBuffer.ringBufferSize() << " / " << msInBuffer << " [ms]" << endl;
    cout << " - Active input channels: ";
    foreach (unsigned int id, audioBuffer.ringBuffer.channelIds) cout << "Ch" << id << " ";
    cout << endl;

    try {
        rtAudio->openStream(NULL, &parameters, settings.format, settings.hwSampleRate, &settings.hwBufferSize, &AudioCallback::interleaved, this);
        rtAudio->startStream();
        processingIntervalTimer.restart();
    }
#ifdef RTERROR_H
    catch( RtError &error )
#else
    catch( RtAudioError& error )
#endif
    { error.printMessage(); return false; }
    return true;
}

/*
 * Stop and close the active audio stream.
 */
bool AudioStreamer::stopStream()
{
    if( rtAudio->isStreamRunning() ) {
        try { rtAudio->stopStream(); }
#ifdef RTERROR_H
        catch( RtError& error )
#else
        catch( RtAudioError& error )
#endif
        { error.printMessage(); return false; }
    }

    if( rtAudio->isStreamOpen() ) {
        try { rtAudio->closeStream(); }
#ifdef RTERROR_H
        catch( RtError& error )
#else
        catch( RtAudioError& error )
#endif
        { error.printMessage(); return false; };
    }

    return true;
}

/*
 * Refreshes processing buffer, copying actual content of the aquisition buffer and trigger processing.
 * Note: This is called high frequently -> triggered always after each audio callback has finished.
 */
void AudioStreamer::slotUpdateBuffers()
{
    if( rtAudio->isStreamRunning() && rtAudio->isStreamOpen() )
    {
        if( audioBuffer.rawBuffer.grabFramesFromQueue() )
        {
            audioProcessing.slotUpdateRingBuffer(&audioBuffer);

            // trigger processing within processingInterval
            // Hmm .. this is not tight/equidistant enough. Just triggered from audio callback -> jitter :(
            unsigned int msElappsed = (unsigned int)((processingIntervalTimer.nsecsElapsed()/1000000.)+0.5);

            if( msElappsed > processingInterval && audioBuffer.isFilled() ) {
                processingIntervalTimer.restart();
                audioProcessing.slotAudioProcessing();
            }
        }
        else { cerr << "Warning: Detected missing frames .." << endl; }
    }
}
