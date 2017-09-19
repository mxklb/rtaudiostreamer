#include "streamsettings.h"
#include "bufferdefaults.h"
#include <QSettings>
#include <limits>

/*
 * Constructor: Set default values
 */
StreamSettings::StreamSettings()
{
    QSettings settings;
    hwSampleRate = settings.value("audiostreamer/hwSampleRate", 44100).toUInt();
    hwBufferSize = settings.value("audiostreamer/hwBufferSize", bufferdefaults::hwBufferSize).toUInt();
    audioFormat = settings.value("audiostreamer/rtAudioFormat", (unsigned int)0x2).toUInt();
    options.numberOfBuffers = settings.value("audiostreamer/rtNumberOfBuffers", 0).toUInt();
    options.priority = settings.value("audiostreamer/rtPriority", 0).toUInt();
    options.flags = settings.value("audiostreamer/rtAudioOptionFlags", 0).toUInt();
    options.streamName = settings.applicationName().toStdString();
}

/*
 * Returns the maximum value for the actual streaming format.
 */
double StreamSettings::formatLimit()
{
    return formatLimit(audioFormat);
}

/*
 * Returns the maximum value for the given streaming format.
 */
double StreamSettings::formatLimit(RtAudioFormat type)
{
    double defaultValue = 32767; // qint16 = RTAUDIO_SINT16
    switch( type ) {
        case RTAUDIO_SINT8: return std::numeric_limits<qint8>::max();
        case RTAUDIO_SINT16: return std::numeric_limits<qint16>::max();
        case RTAUDIO_SINT32: return std::numeric_limits<qint32>::max();
        case RTAUDIO_FLOAT32: return std::numeric_limits<float>::max();
        case RTAUDIO_FLOAT64: return std::numeric_limits<qreal>::max();
    }
    return defaultValue;
}
