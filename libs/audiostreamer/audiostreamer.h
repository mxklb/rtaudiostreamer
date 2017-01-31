#ifndef AUDIOSTREAMER_H
#define AUDIOSTREAMER_H

#include <RtAudio.h>
#include <QObject>
#include <QTimer>

#include "audiobuffer.h"
#include "streamsettings.h"
#include "audioprocessing.h"

class AudioStreamer : public QObject
{
    Q_OBJECT
public:
    explicit AudioStreamer(QObject *parent = 0);
    ~AudioStreamer();

    void printListOfDevices();
    QList<RtAudio::DeviceInfo> getListOfDevices();

    unsigned int numberOfInputChannels(int deviceId = -1);
    QList<unsigned int> getInputChannelIds(int deviceId = -1);

    void setActiveDevice(unsigned int deviceId, QList<unsigned int> channels = QList<unsigned int>());
    void setActiveChannels(QList<unsigned int> channels);

    bool startStream(StreamSettings settings = StreamSettings());
    bool stopStream();

signals:
    void triggerAudioProcessing(AudioBuffer *audioData);

private:
    RtAudio* rtAudio;
    unsigned int activeDeviceId;
    QList<RtAudio::DeviceInfo> devices;

    void setupDeviceList();

    AudioBuffer acquisitionBuffer;
    AudioBuffer processingBuffer;
    void allocateRingBuffers(QList<unsigned int> channels, unsigned int size = 8192);

    QTimer *processingTimer;
    AudioProcessing audioProcessing;

private slots:
    void slotUpdateProcessingBuffer();
};

#endif // AUDIOSTREAMER_H
