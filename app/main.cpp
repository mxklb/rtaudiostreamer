#include <QCoreApplication>
#include "audiostreamer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("xaMBlak");
    QCoreApplication::setOrganizationDomain("github.com/mxklb");
    QCoreApplication::setApplicationName("RtAudioStreamer");

    AudioStreamer streamer;
    streamer.printListOfDevices();

    unsigned int deviceId = 0;
    QVector<unsigned int> channels({0,1});

    streamer.setActiveDevice(deviceId, channels);
    streamer.startStream();

    return a.exec();
}
