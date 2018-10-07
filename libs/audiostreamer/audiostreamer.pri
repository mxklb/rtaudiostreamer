INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

macx {
    INCLUDEPATH += /usr/local/include/
    LIBS += -L/usr/local/lib -lrtaudio
}

INCLUDEPATH += /usr/include/rtaudio/
