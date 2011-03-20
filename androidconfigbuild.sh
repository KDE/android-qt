#!/bin/sh

#default values

PLATFORM="-platform linux-g++"
NDK_ROOT=/usr/local/android-ndk-r5
NDK_HOST=linux-x86

if [ "$OSTYPE" = "msys" ]; then
	PLATFORM="-platform win32-g++"
	NDK_ROOT=c:/android/android-ndk-r5b
	NDK_HOST=windows
fi

NDK_TOOLCHAIN_PREFIX=arm-linux-androideabi
NDK_TOOLCHAIN_VERSION=4.4.3
TARGET_ARCH=armeabi
          # armeabi - tune for android arm v5
          # armeabi-v7a - tune for android arm v7

CONFIGURE_QT=0
PATCH_QT=1
COMPILATION_TYPE=1

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
   -a      Target cpu architecture. Default "$TARGET_ARCH"
                   armeabi     - tune for android arm v5
                   armeabi-v7a - tune for android arm v7
   -q      Qt build options.
                   0 - don't configure qt (only compile) default
                   1 - configure qt and compile qt
   -d      Build type. Default "$COMPILATION_TYPE"
                   1 - Compile and link Qt with debugging turned on.
                   0 - Compile and link Qt using release compilation flags, it also
                   contains debugging informations.
   -c      Patch qt.
                   0 - don't patch qt (used to make the installer)
                   1 - patch qt (default)
   -i      Install path. Default "$QT_INSTALL_DIR"
EOF
}


while getopts "help:r:h:p:v:a:q:c:i:d:" arg; do
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
	a)
		TARGET_ARCH=$OPTARG
		;;
	q)
		CONFIGURE_QT=$OPTARG
		;;
	c)
		PATCH_QT=$OPTARG
		;;
	i)
		QT_INSTALL_DIR=$OPTARG
		;;
	d)
		COMPILATION_TYPE=$OPTARG
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
		rm -fr plugins
		rm -fr lib
		git checkout lib
		rm -fr imports
		git checkout imports
	fi

	CTYPE=-debug
	if [ $COMPILATION_TYPE = 0 ]
	then
		$CTYPE= -release -reduce-relocations -reduce-exports 
	fi
	
	$QT_SRC_DIR/configure -v -opensource $CTYPE -qpa -arch arm \
		-no-phonon -freetype -fast -xplatform android-g++ \
		$PLATFORM -host-little-endian \
		-little-endian -no-qt3support -no-largefile \
		--prefix=$QT_INSTALL_DIR \
		-openssl -shared -pch \
		-nomake demos -nomake examples -confirm-license -exceptions \
		-webkit -script || exit 1
fi

make -j3 || exit 1

if [ $PATCH_QT = 1 ]
then
    $QT_SRC_DIR/qpatch $QT_SRC_DIR/files-to-patch-android $QT_INSTALL_DIR $PWD
fi

#INSTALL_ROOT=$QT_SRC_DIR/qt/$TARGET_ARCH make install
