#!/bin/sh

# ./configure -embedded-lite arm -no-phonon -freetype -developer-build -static -fast -xplatform android-g++ -little-endian -no-webkit -no-qt3support -no-largefile -no-stl -no-javascript-jit -no-script -confirm-license -opengl es1 #
./configure -opensource -release -embedded-lite arm -no-phonon -freetype -fast -xplatform android-g++ -little-endian -no-qt3support -no-largefile -no-exceptions --prefix=/data/local/qt -static -nomake demos -nomake examples -no-opengl -confirm-license -v  -no-webkit
