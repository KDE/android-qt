#!/bin/sh

make -j3
adb shell rm -r /data/local/qt
rm -fr al
mkdir al
cp -a lib/*.so* al/
adb push al /data/local/qt/lib
