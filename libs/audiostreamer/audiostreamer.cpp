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
    unsigned int processingInterval = settings.value("audiostreamer/processingInterval", 25).toUInt();
    unsigned int processingBufferSize = settings.value("audiostreamer/processingBufferSize", 8192).toUInt();

    try {
        rtAudio = new RtAudio();
        activeDeviceId = settings.value("audiostreamer/activeDeviceId", rtAudio->getDefaultInputDevice()).toUInt();
    }
    catch (RtAudioError &error) {
        error.printMessage();
    }

    setupDeviceList();
    printListOfDevices();
    allocateRingBuffers( getInputChannelIds(activeDeviceId), processingBufferSize );

    // todo audioProcessing should be independant lib connected form outside ..
    connect(this, SIGNAL(triggerAudioProcessing(AudioBuffer*)), &audioProcessing, SLOT(processAudio(AudioBuffer*)));

    processingTimer = new QTimer(this);
    connect(processingTimer, SIGNAL(timeout()), this, SLOT(slotUpdateProcessingBuffer()));
    processingTimer->start(processingInterval);
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
    for (unsigned int i=0; i<rtAudio->getDeviceCount(); i++) {
        try {
            RtAudio::DeviceInfo info = rtAudio->getDeviceInfo(i);
            devices.push_back(info);
        }
        catch (RtAudioError &error) {
            error.printMessage();
            break;
        }
    }
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
QList<unsigned int> AudioStreamer::getInputChannelIds(int deviceId)
{
    QList<unsigned int> channels;
    for( unsigned int i=0; i<numberOfInputChannels(deviceId); i++ ) {
        channels.push_back(i);
    }
    return channels;
}

/*
 * Returns the number of input channels for the given device.
 * If the given id <= -1 returns the input channels of the active device.
 */
unsigned int AudioStreamer::numberOfInputChannels(int deviceId)
{
    unsigned int id = activeDeviceId;
    if( deviceId > -1 && deviceId < devices.size() )
        id = deviceId;
    return devices.at(id).inputChannels;
}

/*
 * Allocates ringbuffers for aquisition and processing of each channel for the active device.
 */
void AudioStreamer::allocateRingBuffers(QList<unsigned int> channels, unsigned int size)
{
    acquisitionBuffer.allocateRingbuffers(size, channels);
    processingBuffer.allocateRingbuffers(size, channels);
}

/*
 * Changes the channel ids used for audio streaming.
 */
void AudioStreamer::setActiveChannels(QList<unsigned int> channels)
{
    setActiveDevice(activeDeviceId, channels);
}

/*
 * Switches the audio device to be used. Stops running stream and resets buffers.
 */
void AudioStreamer::setActiveDevice(unsigned int id, QList<unsigned int> channels)
{
    stopStream();
    activeDeviceId = id;
    if( channels.isEmpty() ) channels = getInputChannelIds();
    allocateRingBuffers(channels, acquisitionBuffer.ringBufferSize);
}

/*
 * Opens and starts the audio stream to be prossessed by audioprocessor callback.
 */
bool AudioStreamer::startStream(StreamSettings settings)
{
    if( acquisitionBuffer.numberOfChannels() == 0 ) {
        cout << "Warning: Unable to start stream without input channels!" << endl;
        return false;
    }

    unsigned int firstChannelId = acquisitionBuffer.activeChannelIds.first();
    unsigned int lastChannelId = acquisitionBuffer.activeChannelIds.last();

    RtAudio::StreamParameters parameters = settings.parameters;
    parameters.nChannels = lastChannelId - firstChannelId + 1;
    parameters.deviceId = activeDeviceId;
    parameters.firstChannel = firstChannelId;

    double msInBuffer = 1000. * acquisitionBuffer.ringBufferSize / (double)settings.hwSampleRate;

    cout << "Starting audio stream of " << parameters.nChannels << " channels for device [" << activeDeviceId << "], buffer " << msInBuffer << " [ms]" << endl;
    cout << " - Active channels: ";
    foreach (unsigned int id, acquisitionBuffer.activeChannelIds) cout << "Ch" << id << " ";
    cout << endl;

    try { // todo: make audio format a config option
      rtAudio->openStream(NULL, &parameters, RTAUDIO_SINT16, settings.hwSampleRate, &settings.hwBufferSize, &audioStreamCallback, &acquisitionBuffer);
      rtAudio->startStream();
    }
    catch ( RtAudioError& e ) {
      e.printMessage();
      return false;
    }
    return true;
}

/*
 * Stop and close the active audio stream.
 */
void AudioStreamer::stopStream()
{
    if( rtAudio->isStreamRunning() ) try { rtAudio->stopStream(); } catch ( RtAudioError& e ) { e.printMessage(); };
    if( rtAudio->isStreamOpen() ) try { rtAudio->closeStream(); } catch ( RtAudioError& e ) { e.printMessage(); };
}

/*
 * Refreshes processing buffer, copying actual content of the aquisition buffer and trigger processing.
 */
void AudioStreamer::slotUpdateProcessingBuffer()
{
    if( rtAudio->isStreamRunning() && rtAudio->isStreamOpen() ) {
        if( acquisitionBuffer.frameCounter / acquisitionBuffer.ringBufferSize > 1 ) {
            processingBuffer = acquisitionBuffer;
            cout << acquisitionBuffer.frameCounter;
        }
        emit triggerAudioProcessing(&processingBuffer);
    }
}
