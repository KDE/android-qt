#!/bin/bash

# Warning :: this will take ages.

# Execute this from a folder where you want to build Qt for Android, e.g.
# having my Git repo in a folder called QtNecessitasSrc in the current directory:
# mkdir QtNecessitasBuild; cd QtNecessitasBuild; ../QtNecessitasSrc/build.sh

if [ "$1" = "-p" ]; then
	`dirname $0`/androidconfigbuild.sh -p
	exit 0
fi

if [ "$OSTYPE" = "msys" ]; then
	NDKHOST="windows"
        NDKDIR="/usr/android-sdk-windows/android-ndk-r5b"
	DEST_DIR_QT=C:/Necessitas/4.8.0
else
	NDKHOST="linux-x86"
	# On Linux, with ~ in the NDK path, g++ fails to find stdint.h (-I~/android/android-sdk... fail?!)
	# Not what you'd expect! This should fix it.
	NDKDIR=~/android/android-sdk-linux_x86/android-ndk-r5b
	pushd . > /dev/null
	cd $NDKDIR
	NDKDIR=`pwd`
	popd > /dev/null
	echo "NDKDIR is $NDKDIR"
	DEST_DIR_QT=/data/data/eu.licentia.necessitas.ma.ministro/files/qt
fi

# Makes {shared}*{debug,release}*{exceptions,noexceptions}, all armv7-a, and no thumb.
# `dirname $0`/androidconfigbuild.sh -l 1 -q 1 -n $NDKDIR -o $NDKHOST -f arm-linux-androideabi -v 4.4.3 -a armeabi-v7a \
#          -h 1 -x 1 -d 0 -r 1 -m 1 -i $DEST_DIR_QT
#`dirname $0`/androidconfigbuild.sh -l 1 -q 1 -n $NDKDIR -o $NDKHOST -f arm-linux-androideabi -v 4.4.3 -a armeabi-v7a \
#         -h 1 -x 1 -d 1 -r 0 -m 1 -i $DEST_DIR_QT
#Without exceptions get: QtXmlPatterns was requested, but it can't be built due to exceptions being disabled.
#`dirname $0`/androidconfigbuild.sh -l 1 -q 1 -n $NDKDIR -o $NDKHOST -f arm-linux-androideabi -v 4.4.3 -a armeabi-v7a \
#         -h 1 -x 0 -d 0 -r 1 -m 1 -i $DEST_DIR_QT
#`dirname $0`/androidconfigbuild.sh -l 1 -q 1 -n $NDKDIR -o $NDKHOST -f arm-linux-androideabi -v 4.4.3 -a armeabi-v7a \
#         -h 1 -x 0 -d 1 -r 0 -m 1 -i $DEST_DIR_QT

`dirname $0`/androidconfigbuild.sh -l 1 -q 1 -n $NDKDIR -o $NDKHOST -f arm-linux-androideabi -v 4.4.3 -a armeabi-v7a \
         -h 1 -x 1 -d 0 -r 1 -m 0 -i $DEST_DIR_QT
