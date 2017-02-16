#include "streamsettings.h"

#include <QSettings>
#include <limits>

StreamSettings::StreamSettings()
{
    QSettings settings;
    hwSampleRate = settings.value("audiostreamer/hwSampleRate", 44100).toUInt();
    hwBufferSize = settings.value("audiostreamer/hwBufferSize", 256).toUInt();
    format = settings.value("audiostreamer/rtAudioFormat", (int)RTAUDIO_SINT16).toUInt();
}

/*
 * Returns the maximum value for the actual streaming format.
 */
double StreamSettings::formatLimit()
{
    return formatLimit(format);
}

/*
 * Returns the maximum value for the given streaming format.
 */
double StreamSettings::formatLimit(RtAudioFormat type)
{
    double defaultValue = 32768;
    switch( type ) {
        case RTAUDIO_SINT8: return std::numeric_limits<qint8>::max()+1;
        case RTAUDIO_SINT16: return std::numeric_limits<qint16>::max()+1;
        case RTAUDIO_SINT32: return std::numeric_limits<qint32>::max()+1;
        case RTAUDIO_FLOAT32: return std::numeric_limits<float>::max()+1;
        case RTAUDIO_FLOAT64: return std::numeric_limits<qreal>::max()+1;
    }
    return defaultValue;
}
