#!/bin/sh

make -j3 || exit 0
adb shell rm -r /data/local/qt || exit 0
rm -fr al
mkdir al
cp -a lib/*.so* al/

adb push al /data/local/qt/lib
adb push imports /data/local/qt/imports
adb push plugins /data/local/qt/plugins
