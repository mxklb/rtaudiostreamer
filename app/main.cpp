#include <QCoreApplication>
#include "audiostreamer.h"
#include <QThread>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("xaMBlak");
    QCoreApplication::setOrganizationDomain("github.com/mxklb");
    QCoreApplication::setApplicationName("RtAudioStreamer");

    AudioStreamer streamer;
    streamer.printListOfDevices();

    unsigned int deviceId = 0;
    QList<unsigned int> channels({0,1});

    streamer.setActiveDevice(deviceId, channels);
    streamer.startStream();

    //std::cerr << "thread ID (main): " << QThread::currentThreadId();

    return a.exec();
}
