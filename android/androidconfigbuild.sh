#!/bin/bash -x

#defaults:
CLEAN_QT=1
CONFIGURE_QT=1
BUILD_QT=1
INSTALL_QT=0
SHARED_QT=1
EXCEPTIONS_QT=1
DEBUG_QT=0
MODIFY_DEST_DIR_QT=0
DOWNLOAD_OPENSSL=1
NDK_PLATFORM=5
DEST_DIR_QT=$PWD
PLATFORM="-platform linux-g++"
NDK_ROOT=~/necessitas/android-ndk
NDK_HOST=linux-x86
NDK_TOOLCHAIN_VERSION=4.6
NDK_TOOLCHAIN_PREFIX=arm-linux-androideabi
NDK_TOOLS_PREFIX=arm-linux-androideabi
TARGET_ARCH=armeabi-v7a
ANDROID_ARCHITECTURE=arm

OSTYPE_MAJOR=${OSTYPE/%[0-9.]*/}
SRC_DIR_QT=`dirname $0`
pushd .
SRC_DIR_QT=`(cd "$SRC_DIR_QT"; /bin/pwd)`
popd
SRC_DIR_QT=$SRC_DIR_QT/..
echo $SRC_DIR_QT

#openssl vars
OPENSSL_PACKAGE="openssl-1.0.0d.tar.gz"
OPENSSL_NAME=`echo $OPENSSL_PACKAGE|sed 's#\.tar.gz##g'`
OPENSSL_URL=http://www.openssl.org/source/$OPENSSL_PACKAGE
OPENSSL_PREFIX=$SRC_DIR_QT/src/3rdparty/android/src/openssl
OPENSSL_ROOT=$OPENSSL_PREFIX/$OPENSSL_NAME

if [ "$OSTYPE_MAJOR" = "msys" ]; then
	PLATFORM="-platform win32-g++"
	NDK_HOST=windows
	NDK_PLATFORM=4
else
	if [ "$OSTYPE_MAJOR" = "darwin" ]; then
		PLATFORM="-platform macx-g++"
		NDK_ROOT=/usr/local/android/android-ndk-r6b
		NDK_HOST=darwin-x86
		NDK_PLATFORM=4
	fi
fi

help()
{
cat << EOF
usage: $0 options

OPTIONS:
   -p      Shows this message
   -c      Clean qt
   -q      Qt build options
                   0 - don't configure qt (only compile) default
                   1 - configure qt and compile qt
   -b      Build qt
                   0 - don't build
                   1 - build
   -k      Install qt
                   0 - don't install
                   1 - install
   -n      NDK root. Default "$NDK_ROOT"
   -o      NDK host. Default "$NDK_HOST"
   -f      NDK toolchain prefix. Default "$NDK_TOOLCHAIN_PREFIX"
   -v      NDK toolchain version. Default "$NDK_TOOLCHAIN_VERSION"
   -a      Target cpu architecture. Default "$TARGET_ARCH"
                   armeabi     - tune for android arm v5
                   armeabi-v7a - tune for android arm v7
                   x86         - tune for android x86
                   Optional name suffix: v5/v7
   -l      Android API Level. Default "$NDK_PLATFORM"
   -h      Shared
                   0 - Build static version of qt
                   1 - Build shared version of qt
   -x      Exceptions options
                   0 - don't use exceptions (you won't be able to build qtcreator with such a qt though)
                   1 - use exceptions
                   Optional name suffix: Xc/Nx
   -d      Build debug qt
                   Optional name suffix: D/R/DaR
   -m      Modify install path according to passed in options (name suffixes applied in above listed order).

   -i      Install path. Default "$DEST_DIR_QT"
   -w      Download the OpenSSL sources from "$OPENSSL_URL and
           extract to "$OPENSSL_PREFIX"
              0 - don't download
              1 - download
EOF
}

INSTSUFFIX=""
CFGOPTIONS=""
while getopts "p:c:q:b:k:n:o:f:v:a:l:h:x:d:r:m:i:w:" arg; do
case $arg in
	p)
		help
		exit 0
		;;
	c)
		CLEAN_QT=$OPTARG
		;;
	q)
		CONFIGURE_QT=$OPTARG
		;;
	b)
		BUILD_QT=$OPTARG
		;;
	k)
		INSTALL_QT=$OPTARG
		;;
	n)
		NDK_ROOT=$OPTARG
		;;
	o)
		NDK_HOST=$OPTARG
		;;
	f)
		NDK_TOOLCHAIN_PREFIX=$OPTARG
		;;
	v)
		NDK_TOOLCHAIN_VERSION=$OPTARG
		;;
	a)
		TARGET_ARCH=$OPTARG
		;;
	h)
		SHARED_QT=$OPTARG
		if [ "$SHARED_QT" -eq "1" ]; then
			INSTSUFFIX="${INSTSUFFIX}Sh"
			CFGOPTIONS="${CFGOPTIONS} -shared "
		else
			INSTSUFFIX="${INSTSUFFIX}St"
			CFGOPTIONS="${CFGOPTIONS} -static "
		fi
		;;
    l)
	    NDK_PLATFORM=$OPTARG
		;;
	x)
		EXCEPTIONS_QT=$OPTARG
		if [ "$EXCEPTIONS_QT" -eq "1" ]; then
			INSTSUFFIX="${INSTSUFFIX}Xc"
			CFGOPTIONS="${CFGOPTIONS} -exceptions "
		else
			INSTSUFFIX="${INSTSUFFIX}Nx"
			CFGOPTIONS="${CFGOPTIONS} -no-exceptions "
		fi
		;;
	d)
		DEBUG_QT=$OPTARG
		;;
	i)
		DEST_DIR_QT=$OPTARG
		;;
	m)
		MODIFY_DEST_DIR_QT=$OPTARG
		;;
	w)
		DOWNLOAD_OPENSSL=$OPTARG
		;;
	?)
		help
		exit 0
		;;
esac
done

if [ "$TARGET_ARCH" = "x86" ]; then
	NDK_TOOLCHAIN_PREFIX="x86"
	NDK_TOOLS_PREFIX="i686-android-linux"
	ANDROID_ARCHITECTURE="x86"
        NDK_PLATFORM=9
fi

if [ "$DOWNLOAD_OPENSSL" -eq "1" ]; then
	if [ ! -d "$OPENSSL_ROOT" ]; then
		echo "Downloading OpenSSL sources from $OPENSSL_URL."
        if [ "$OSTYPE_MAJOR" = "darwin" ] ; then
            pushd "$OPENSSL_PREFIX"
            curl --insecure -S -L -O "$OPENSSL_URL"
            popd
        else
		wget "$OPENSSL_URL" --no-verbose --directory-prefix="$OPENSSL_PREFIX"
        fi
		echo "Extracting OpenSSL sources to $OPENSSL_PREFIX."
		tar -xzf "$OPENSSL_PREFIX/$OPENSSL_PACKAGE" -C "$OPENSSL_PREFIX"
		echo "Cleaning up $OPENSSL_PREFIX/$OPENSSL_PACKAGE."
		rm "$OPENSSL_PREFIX/$OPENSSL_PACKAGE"
		echo "Patching $OPENSSL_ROOT/crypto/bio/bss_fd.c"
		echo "#ifdef INCLUDE_BSS_FILE" >> $OPENSSL_ROOT/crypto/bio/bss_fd.c
		echo "#include \"bio/bss_file.c\"" >> $OPENSSL_ROOT/crypto/bio/bss_fd.c
		echo "#endif" >> $OPENSSL_ROOT/crypto/bio/bss_fd.c
	else
		echo "Found existing OpenSSL sources in $OPENSSL_ROOT."
	fi
fi

if [ "$DEBUG_QT" -eq "1" ]; then
	CFGOPTIONS="${CFGOPTIONS} -debug -developer-build "
	INSTSUFFIX="${INSTSUFFIX}D"
else
	CFGOPTIONS="${CFGOPTIONS} -release -reduce-relocations -reduce-exports "
	INSTSUFFIX="${INSTSUFFIX}R"
fi

if [ "$MODIFY_DEST_DIR_QT" -eq "1" ]; then
	DEST_DIR_QT=${DEST_DIR_QT}-${INSTSUFFIX}
fi

# Should add an ANDROID_MIN_PLATFORM qmake var that
# NQTC can read and use to prevent selecting earlier platforms.
#if [ "$TARGET_ARCH" = "armeabi-v7a" ]; then
#    NDK_PLATFORM=5
#fi

echo "New install of Qt will be to $DEST_DIR_QT"

# Please set the following evn vars correctly !!!
export ANDROID_NDK_ROOT=$NDK_ROOT
export ANDROID_NDK_HOST=$NDK_HOST
export ANDROID_NDK_TOOLCHAIN_PREFIX=$NDK_TOOLCHAIN_PREFIX
export ANDROID_NDK_TOOLCHAIN_VERSION=$NDK_TOOLCHAIN_VERSION
export ANDROID_NDK_TOOLS_PREFIX=$NDK_TOOLS_PREFIX
export ANDROID_TARGET_ARCH=$TARGET_ARCH
export ANDROID_PLATFORM=$NDK_PLATFORM
export ANDROID_NDK_PLATFORM=android-$NDK_PLATFORM

cat << EOF
Compiling qt with the following env vars:
ANDROID_NDK_ROOT=$NDK_ROOT
ANDROID_NDK_HOST=$NDK_HOST
ANDROID_NDK_TOOLCHAIN_PREFIX=$NDK_TOOLCHAIN_PREFIX
ANDROID_NDK_TOOLCHAIN_VERSION=$NDK_TOOLCHAIN_VERSION
ANDROID_PLATFORM=$NDK_PLATFORM
ANDROID_NDK_PLATFORM=android-$NDK_PLATFORM
ANDROID_TARGET_ARCH=$TARGET_ARCH
EOF

# Without this, make will not be able to translate relative paths
# properly as it can't step beyond where / is mounted.
if [ "$OSTYPE_MAJOR" = "msys" ]; then
	MAKEDIR=`pwd -W`
	MAKEFILE=$MAKEDIR/Makefile
else
	MAKEFILE=Makefile
fi

if [ "$CLEAN_QT" -eq "1" ]; then
	if [ -f Makefile ]; then
		make -f $MAKEFILE distclean
	fi
	if [ -d qmake ]; then
		pushd .
		cd qmake
		if [ -f Makefile ]; then
			make -f $MAKEFILE clean
		fi
		popd
	fi
fi

if [ $CONFIGURE_QT = 1 ]
then
	if [ $SRC_DIR_QT = $PWD ]
	then
		make -f $MAKEFILE confclean distclean
		rm -fr include
		rm -fr plugins
		rm -fr lib
		git checkout lib
		rm -fr imports
		git checkout imports
	fi

        if [ "$OSTYPE_MAJOR" = "darwin" ]; then
            HOST_ARCH="macosx"
        else
            HOST_ARCH="i386"
        fi

	cp -f $NDK_ROOT/sources/cxx-stl/gnu-libstdc++/$NDK_TOOLCHAIN_VERSION/libs/$TARGET_ARCH/libgnustl_shared.so $SRC_DIR_QT/lib

	$SRC_DIR_QT/configure -v -opensource -qpa -arch $ANDROID_ARCHITECTURE \
		-no-phonon -freetype -fast -xplatform android-g++ \
		-host-arch $HOST_ARCH $PLATFORM -host-little-endian \
		-little-endian -no-qt3support -no-largefile \
		-openssl -pch \
		-nomake demos -no-multimedia -nomake examples -confirm-license \
		$CFGOPTIONS -prefix $DEST_DIR_QT \
		-openssl-source $OPENSSL_ROOT \
                -script -no-webkit -stl || exit 1
fi

# This should loop until make succeeds, Workaround for Cygwin/MSYS
# couldn't commit heap memory error
if [ "$BUILD_QT" = "1" ]; then
	make -f $MAKEFILE -j 6 all QtJar
	if [ "$OSTYPE" = "msys" ] ; then
		while [ "$?" != "0" ]
		do
			if [ -f /usr/break-make ]; then
				echo "Detected break-make"
				rm -f /usr/break-make
				exit 1
			fi
			make -f $MAKEFILE -j 6 all QtJar
		done
	fi
fi


if [ "$INSTALL_QT" = "1" ] ; then
	make -f $MAKEFILE install
	if [ "$OSTYPE" = "msys" ] ; then
		while [ "$?" != "0" ]
		do
			if [ -f /usr/break-make ]; then
				echo "Detected break-make"
				rm -f /usr/break-make
				exit 1
			fi
			make -f $MAKEFILE install
		done
	fi
	# Controversial - lets not do it...
	# $SRC_DIR_QT/copy-private-headers.sh include $DEST_DIR_QT/private-headers
fi

exit 0
