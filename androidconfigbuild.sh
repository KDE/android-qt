#!/bin/bash

#configure parses mkspecs/android-g++/qmake.conf to figure out
#the compiler (and various other bits) using getQMakeConf:
#QMAKE_CONF_COMPILER=`getQMakeConf "$XQMAKESPEC" | grep "^QMAKE_CXX[^_A-Z0-9]" | sed "s,.* *= *\(.*\)$,\1," | tail -1`
#this returns:
#$$NDK_TOOLCHAIN_PATH/bin/$$NDK_TOOLCHAIN_PREFIX-g++
#Which gets passed through to various shell scripts, as is
#and they fail to run. To fix, need to also do:
#QMAKE_CONF_COMPILER=${QMAKE_CONF_COMPILER//\$\$/\$} # replace all $$ with $
#eval QMAKE_CONF_COMPILER=$QMAKE_CONF_COMPILER       # evaluate the variables
#However, these variables aren't set at configure time, so set them now (and
#export them so configure can use them.

#default values
export ANDROID_PLATFORM=android-9
export NDK_PLATFORM=9
export NDK_TOOLCHAIN_PREFIX=arm-linux-androideabi
export NDK_TOOLCHAIN_VERSION=4.4.3
export ANDROID_TARGET_ARCH=armeabi-v7a

TARGET_ARCH=armeabi-v7a

CONFIGURE_QT=0
PATCH_QT=1
COMPILATION_TYPE=1

if [ "$OSTYPE" = "msys" ]; then
	PLATFORM="-platform win32-g++"
	# Building webkit we run into cmd line length limits when linking.
	# 32k odd characters in it easily.
	# Paths are along the lines of .obj/debug_shared/JSHTMLSomethingVeryVerbose.o
	# The only thing we can really do is ".ods/JSHTMLSomethingVeryVerbose.o" and
	# then hope. But not sure how to achieve that yet...
	# This doens't do anything. I had to hack configure to make it drop webkit.
	NOWEBKIT="-no-webkit"
	set NDK_ROOT=/usr/android-sdk-windows/android-ndk-r5b
	NDK_ROOT=/usr/android-sdk-windows/android-ndk-r5b
	set NDK_HOST=windows
	NDK_HOST=windows
	set ANDROID_NDK_HOST=$NDK_HOST
	export ANDROID_NDK_HOST=$NDK_HOST
	set NDK_TOOLCHAIN_PATH=$NDK_ROOT/toolchains/$NDK_TOOLCHAIN_PREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$NDK_HOST
	NDK_TOOLCHAIN_PATH=$NDK_ROOT/toolchains/$NDK_TOOLCHAIN_PREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$NDK_HOST
	set ANDROID_NDK_ROOT=$NDK_ROOT
	export ANDROID_NDK_ROOT=$NDK_ROOT
	export NDK_TOOLCHAIN_PATH=$NDK_ROOT/toolchains/$NDK_TOOLCHAIN_PREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$NDK_HOST
else
	export NDK_ROOT=/home/ray/android/android-sdk-linux_x86/android-ndk-r5b
	export NDK_HOST=linux-x86
	export ANDROID_NDK_HOST=$NDK_HOST
	export NDK_TOOLCHAIN_PATH=$NDK_ROOT/toolchains/$NDK_TOOLCHAIN_PREFIX-$NDK_TOOLCHAIN_VERSION/prebuilt/$NDK_HOST
	export ANDROID_NDK_ROOT=$NDK_ROOT
fi


QT_INSTALL_DIR=/usr/latest-git/android-qt/mingw-android-lighthouse-build-out
QT_SRC_DIR=`dirname $0`
pushd .
QT_SRC_DIR=`(cd "$QT_SRC_DIR"; /bin/pwd)`
popd

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
		$NOWEBKIT \
		-little-endian -no-qt3support -no-largefile \
		--prefix=$QT_INSTALL_DIR \
		-openssl -shared -pch \
		-nomake demos -nomake examples -confirm-license -exceptions \
		-webkit -script || exit 1
fi

# make -j9 || exit 1

# This should loop until make succeeds, Workaround for Cygwin/MSYS
# couldn't commit heap memory error
# make -j9
# while [ "$?" = "0" ]
# do
#    make -j9
# done

if [ $PATCH_QT = 1 ]
then
    if [ "$OSTYPE" = "msys" ]; then
        $QT_SRC_DIR/qpatch$EXEEXT $QT_SRC_DIR/files-to-patch-android-mingw $QT_INSTALL_DIR $PWD
    else
        $QT_SRC_DIR/qpatch $QT_SRC_DIR/files-to-patch-android $QT_INSTALL_DIR $PWD
    fi
fi

#INSTALL_ROOT=$QT_SRC_DIR/qt/$TARGET_ARCH make install

