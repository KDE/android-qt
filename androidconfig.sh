#!/bin/sh

# ./configure -embedded-lite arm -no-phonon -freetype -developer-build -static -fast -xplatform android-g++ -little-endian -no-webkit -no-qt3support -no-largefile -no-stl -no-javascript-jit -no-script -confirm-license -opengl es1
./configure -opensource -release -embedded-lite arm -no-phonon -freetype -static -fast -xplatform android-g++ -little-endian -no-webkit -no-qt3support -no-largefile -no-stl -no-javascript-jit -no-script -confirm-license
