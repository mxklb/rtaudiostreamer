#!/bin/bash
pushd `dirname $0` > /dev/null
scriptPath=`pwd`
popd > /dev/null
cd "$scriptPath"

# Prepare RtAudioStreamer deployment
macdeployqt $scriptPath/../app/RtAudioStreamer.app

install_name_tool -id @executable_path/../Frameworks/audiobuffer.framework/Versions/1/audiobuffer $scriptPath/../app/RtAudioStreamer.app/Contents/Frameworks/audiobuffer.framework/Versions/1/audiobuffer
install_name_tool -change $scriptPath/../app/RtAudioStreamer.app/Contents/Frameworks/audiobuffer.framework/Versions/1/audiobuffer @executable_path/../Frameworks/audiobuffer.framework/Versions/1/audiobuffer $scriptPath/../app/RtAudioStreamer.app/Contents/MacOs/RtAudioStreamer

#install_name_tool -id @executable_path/../Frameworks/audiostreamer.framework/Versions/1/audiostreamer $scriptPath/../app/RtAudioStreamer.app/Contents/Frameworks/audiostreamer.framework/Versions/1/audiostreamer
#install_name_tool -change $scriptPath/../app/RtAudioStreamer.app/Contents/Frameworks/audiostreamer.framework/Versions/1/audiostreamer @executable_path/../Frameworks/audiostreamer.framework/Versions/1/audiostreamer $scriptPath/../app/RtAudioStreamer.app/Contents/MacOs/RtAudioStreamer