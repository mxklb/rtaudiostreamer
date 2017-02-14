#ifndef AUDIOPROCESSING_H
#define AUDIOPROCESSING_H

#include <QObject>
#include "audiobuffer.h"

class AudioProcessing : public QObject
{
    Q_OBJECT
public:
    explicit AudioProcessing(QObject *parent = 0);

    static QList<double> absoluteAmplitudes(AudioBuffer *buffer);
    static QList<double> logAmplitudes(QList<double> amplitudes, double factor = 10.);

public slots:
    void slotUpdateRingBuffer(AudioBuffer *buffer);
    void slotAudioProcessing();

private:
    AudioBuffer audioBuffer;
    bool updateRingBuffers(QVector<signed short> *rawData, unsigned int numOfChannels);
};

#endif // AUDIOPROCESSING_H
