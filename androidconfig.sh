#!/bin/sh

./configure -opensource -release -qpa arm -no-phonon -freetype -fast -xplatform android-g++ -little-endian -no-qt3support -no-largefile --prefix=/data/local/qt -shared -nomake demos -nomake examples -confirm-license -pch -no-webkit -no-script
