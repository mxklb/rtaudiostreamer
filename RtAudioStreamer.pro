TEMPLATE = subdirs
#CONFIG += ordered

# Sub modules
SUBDIRS = \
    audiobuffer \
    audiostreamer \
    app

# Set directories
app.subdir = app
audiobuffer.subdir = libs/audiobuffer
audiostreamer.subdir = libs/audiostreamer

# Setup dependencies for subdirs
audiostreamer.depends = audiobuffer
app.depends = audiobuffer audiostreamer

include(globals.pri)

# -- Include all test projects
contains(catchTests, true) {
    include(alltests.pri)
    CONFIG(debug, debug|release) {
        message(Debug build with tests detected!)
    } else {
        message(Release build with tests deteced!)
    }
}
