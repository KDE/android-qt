#!/bin/sh

adb shell rm -r /data/local/qt
rm -fr al
make -j2
mkdir al
cp -a lib/*.so* al/
adb push al /data/local/qt/lib
