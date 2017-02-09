#ifndef AUDIOSTREAMER_H
#define AUDIOSTREAMER_H

#include <RtAudio.h>
#include <QElapsedTimer>
#include <QObject>

#include "audiobuffer.h"
#include "streamsettings.h"
#include "audioprocessing.h"

class AudioStreamer : public QObject
{
    Q_OBJECT

    friend class AudioCallback;

public:
    explicit AudioStreamer(QObject *parent = 0);
    ~AudioStreamer();

    void printListOfDevices();
    QList<RtAudio::DeviceInfo> getListOfDevices();

    unsigned int numberOfInputChannels(int deviceId = -1);
    QList<unsigned int> getInputChannelIds(int deviceId = -1);

    void setActiveDevice(unsigned int deviceId, QList<unsigned int> channelIds = QList<unsigned int>());
    void setActiveChannels(QList<unsigned int> channelIds);

    bool startStream(StreamSettings settings = StreamSettings());
    bool stopStream();

protected:
    AudioBuffer* getAudioBuffers() { return &audioBuffer; }
    void callbackFinished() { emit audioCallbackFinished(); }

protected slots:
    void slotUpdateBuffers();

signals:
    void triggerAudioProcessing();
    void grabbedAudioUpdated(AudioBuffer* buffer);
    void audioCallbackFinished();

private:
    RtAudio* rtAudio;
    unsigned int activeDeviceId;
    QList<RtAudio::DeviceInfo> devices;
    StreamSettings streamSettings;

    AudioBuffer audioBuffer;
    AudioProcessing audioProcessing;
    QElapsedTimer processingIntervalTimer;
    unsigned int processingInterval;

    void setupDeviceList();
    void allocateRingBuffers(QList<unsigned int> channels, unsigned int size = 8192);
};

#endif // AUDIOSTREAMER_H
