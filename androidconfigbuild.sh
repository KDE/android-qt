#!/bin/sh

make confclean
rm -fr include
rm -fr lib
git checkout lib

./configure -opensource -release -qpa -arch arm -no-phonon -freetype -fast -xplatform android-g++ -little-endian -no-qt3support -no-largefile --prefix=/data/local/qt -shared -nomake demos -nomake examples -confirm-license -pch -exceptions -no-webkit -no-script -reduce-relocations -reduce-exports
