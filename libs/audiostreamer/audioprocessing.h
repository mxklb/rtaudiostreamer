#ifndef AUDIOPROCESSING_H
#define AUDIOPROCESSING_H

#include <QObject>
#include "audiobuffer.h"
#include "streamsettings.h"

class AudioProcessing : public QObject
{
    Q_OBJECT
public:
    explicit AudioProcessing(QObject *parent = 0);

    StreamSettings streamSettings;

    static QList<double> absoluteAmplitudes(AudioBuffer *buffer, double normalize = 1);
    static QList<double> logAmplitudes(QList<double> amplitudes, double factor = 10.);

    static double accumulate(QVector<double> frames, double norm = 1, bool absolute = true);

public slots:
    void slotUpdateRingBuffer(AudioBuffer *buffer);
    void slotAudioProcessing();

private:
    AudioBuffer audioBuffer;
};

#endif // AUDIOPROCESSING_H
