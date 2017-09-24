#!/bin/bash
# Deploys (pushes) osx dmg images to the osx-deploy branch
#
# Note: Needs GH_TOKEN to auth git push!
pushd `dirname $0` > /dev/null
scriptPath=`pwd`
popd > /dev/null
cd $scriptPath

mkdir dmg
cp $scriptPath/../../app/RtAudioStreamer-*.dmg dmg/RtAudioStreamer.dmg
cp $scriptPath/travis.yml dmg/.travis.yml
cp $scriptPath/README.md dmg/README.md
cd dmg

git init
git config user.name "travis"
git config user.email "travis@email.com"
git add .
git commit -m "OSX deployment - Pushed by travis [$TRAVIS_COMMIT]"
git push --force --quiet "https://${GH_TOKEN}@github.com/mxklb/rtaudiostreamer" master:osx-deploy > /dev/null 2>&1