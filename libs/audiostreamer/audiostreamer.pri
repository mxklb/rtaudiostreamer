INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

macx {
    INCLUDEPATH += /usr/local/include/
    INCLUDEPATH += /usr/local/include/rtaudio/
    LIBS += -L/usr/local/lib -lrtaudio
}

INCLUDEPATH += /usr/include/rtaudio/
