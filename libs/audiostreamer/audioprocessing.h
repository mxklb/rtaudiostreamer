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
    static QList<double> logLoudness(QList<double> amplitudes);

public slots:
    void processAudio(AudioBuffer *buffer);
};

#endif // AUDIOPROCESSING_H
