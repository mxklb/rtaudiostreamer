#ifndef AUDIOSTREAMER_H
#define AUDIOSTREAMER_H

#include <RtAudio.h>
#include <QElapsedTimer>
#include <QObject>
#include <mutex>

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
    QVector<unsigned int> getInputChannelIds(int deviceId = -1);

    void setActiveDevice(unsigned int deviceId, QVector<unsigned int> channelIds = QVector<unsigned int>());
    void setActiveChannels(QVector<unsigned int> channelIds);

    bool startStream(StreamSettings settings = StreamSettings());
    bool stopStream();

protected:
    void callbackFinished();
    bool processLatestAudio();
    AudioBuffer* getAudioBuffer();

protected slots:
    void slotUpdateBuffers();

signals:
    void audioCallbackFinished();
    void rawBufferChanged(AudioBuffer* buffer);

protected:
    QElapsedTimer processingIntervalTimer;
    unsigned int processingInterval;

private:
    RtAudio* rtAudio;
    unsigned int activeDeviceId;
    QList<RtAudio::DeviceInfo> devices;
    StreamSettings streamSettings;

    AudioBuffer audioBuffer;
    AudioProcessing audioProcessing;
    std::mutex mutex;

    void setupDeviceList();
    bool allocateRingBuffers(QVector<unsigned int> listOfchannelIds,
                             unsigned int ringBufferSize = bufferdefaults::ringBufferSize,
                             unsigned int hwBufferSize = bufferdefaults::hwBufferSize);
};

#endif // AUDIOSTREAMER_H
