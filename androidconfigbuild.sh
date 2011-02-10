#!/bin/sh

#default values

NDK_ROOT=/usr/local/android-ndk-r5
NDK_HOST=linux-x86
NDK_TOOLCHAIN_PREFIX=arm-linux-androideabi
NDK_TOOLCHAIN_VERSION=4.4.3
NDK_PLATFORM=8 # 4 - android 1.6
               # 5 - android 2.0 & 2.1
               # 8 - android 2.2
               # 9 - android 2.3
TARGET_ARCH=armeabi
          # armeabi - tune for android arm v5
          # armeabi-v7a - tune for android arm v7

CONFIGURE_QT=0

#QT_INSTALL_DIR=/data/local/qt
QT_INSTALL_DIR=/data/data/eu.licentia.necessitas.ministro/files/qt
QT_SRC_DIR=`dirname $0`
QT_SRC_DIR=`(cd "$QT_SRC_DIR"; /bin/pwd)`

help()
{
cat << EOF
usage: $0 options

OPTIONS:
   -help   Shows this message
   -r      NDK root. Default "$NDK_ROOT"
   -h      NDK host. Default "$NDK_HOST"
   -p      NDK toolchain prefix. Default "$NDK_TOOLCHAIN_PREFIX"
   -v      NDK toolchain version. Default "$NDK_TOOLCHAIN_VERSION"
   -t      Target. Default "$NDK_PLATFORM"
                   4 - android 1.6
                   5 - android 2.0 & 2.1
                   8 - android 2.2
                   9 - android 2.3
   -a      Target cpu architecture. Default "$TARGET_ARCH"
                   armeabi     - tune for android arm v5
                   armeabi-v7a - tune for android arm v7
   -q      Qt build options.
                   0 - don't configure qt (only compile) default
                   1 - configure qt and compile qt
EOF
}


while getopts "help:r:h:p:v:t:a:q:" arg; do
case $arg in
	help)
		help
		exit 0
		;;
	r)
		NDK_ROOT=$OPTARG
		;;
	h)
		NDK_HOST=$OPTARG
		;;
	p)
		NDK_TOOLCHAIN_PREFIX=$OPTARG
		;;
	v)
		NDK_TOOLCHAIN_VERSION=$OPTARG
		;;
	t)
		NDK_PLATFORM=$OPTARG
		;;
	a)
		TARGET_ARCH=$OPTARG
		;;
	q)
		CONFIGURE_QT=$OPTARG
		;;
	?)
		help
		exit 0
		;;
esac
done


# Please set the following evn vars correctly !!!
export ANDROID_NDK_ROOT=$NDK_ROOT
export ANDROID_NDK_HOST=$NDK_HOST
export ANDROID_NDK_TOOLCHAIN_PREFIX=$NDK_TOOLCHAIN_PREFIX
export ANDROID_NDK_TOOLCHAIN_VERSION=$NDK_TOOLCHAIN_VERSION
export ANDROID_NDK_PLATFORM=android-$NDK_PLATFORM
export ANDROID_TARGET_ARCH=$TARGET_ARCH

cat << EOF
Compiling qt with the following env vars:
ANDROID_NDK_ROOT=$NDK_ROOT
ANDROID_NDK_HOST=$NDK_HOST
ANDROID_NDK_TOOLCHAIN_PREFIX=$NDK_TOOLCHAIN_PREFIX
ANDROID_NDK_TOOLCHAIN_VERSION=$NDK_TOOLCHAIN_VERSION
ANDROID_NDK_PLATFORM=android-$NDK_PLATFORM
ANDROID_TARGET_ARCH=$TARGET_ARCH
EOF

if [ $CONFIGURE_QT = 1 ]
then
	if [ $QT_SRC_DIR = $PWD ]
	then
		make confclean distclean
		rm -fr include
		rm -fr lib
		git checkout lib
	fi

	$QT_SRC_DIR/configure -v -opensource -release -qpa -arch arm \
		-no-phonon -freetype -fast -xplatform android-g++ \
		-little-endian -no-qt3support -no-largefile \
		--prefix=$QT_INSTALL_DIR \
		-openssl	-I $QT_SRC_DIR/src/3rdparty/android/precompiled/$ANDROID_NDK_PLATFORM/arch-arm/include \
				-L $QT_SRC_DIR/src/3rdparty/android/precompiled/$ANDROID_NDK_PLATFORM/arch-arm/lib \
		-shared -pch \
		-nomake demos -nomake examples -confirm-license -exceptions \
		-no-webkit -no-script -reduce-relocations -reduce-exports || exit 1
fi

make -j3 || exit 1

$QT_SRC_DIR/bin/qpatch $QT_SRC_DIR/files-to-patch-android $QT_INSTALL_DIR $PWD
