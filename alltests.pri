# Register tests to run
SUBDIRS += \
    audiobuffer-test \
    audiostreamer-test

# Set test apps directories
audiobuffer-test.subdir = libs/audiobuffer/audiobuffer-test
audiostreamer-test.subdir = libs/audiostreamer/audiostreamer-test

# Setup test dependencies
audiobuffer-test.depends = audiobuffer
audiostreamer-test.depends = audiostreamer

# Setup app dependencies to tests
app.depends += audiobuffer-test audiostreamer-test
