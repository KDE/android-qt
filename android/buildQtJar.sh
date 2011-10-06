#!/bin/sh

mkdir -p $2
pushd $1
android update project -n jar -t android-13 -p .
ant clean debug
cp bin/jar-debug.apk $2/
popd
