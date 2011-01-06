#!/bin/sh

adb shell rm -r /data/local/qt || exit 0
rm -fr al
mkdir al
cp -a lib/*.so* al/

/usr/local/android-ndk-r5/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-strip --strip-unneeded al/*

adb push al /data/local/qt/lib
adb push imports /data/local/qt/imports
adb push plugins /data/local/qt/plugins
