#!/bin/bash

echo $0 $@

NDK_TOOLCHAIN_PREFIX=arm-linux-androideabi
NDK_TOOLCHAIN_VERSION=4.4.3
NDK_PLATFORM=9
export ANDROID_PLATFORM=$NDK_PLATFORM

#defaults:
CLEAN_QT=1
CONFIGURE_QT=1
SHARED_QT=1
EXCEPTIONS_QT=0
DEBUG_QT=0
RELEASE_QT=1
MODIFY_DEST_DIR_QT=1
PATCH_QT=0

if [ "$OSTYPE" = "msys" ]; then
	PLATFORM="-platform win32-g++"
	DEST_DIR_QT=C:/Necessitas/4.8.0-pre
else
	PLATFORM="-platform linux-g++"
	DEST_DIR_QT=/data/data/eu.licentia.necessitas.ministro/files/qt
fi

SRC_DIR_QT=`dirname $0`
pushd .
SRC_DIR_QT=`(cd "$SRC_DIR_QT"; /bin/pwd)`
popd

help()
{
cat << EOF
usage: $0 options

OPTIONS:
   -p      Shows this message
   -l      Clean Qt.
   -q      Qt build options.
                   0 - don't configure qt (only compile) default
                   1 - configure qt and compile qt
   -c      Patch qt.
                   0 - don't patch qt (used to make the installer)
                   1 - patch qt (default)
   -n      NDK root. Default "$NDK_ROOT"
   -o      NDK host. Default "$NDK_HOST"
   -f      NDK toolchain prefix. Default "$NDK_TOOLCHAIN_PREFIX"
   -v      NDK toolchain version. Default "$NDK_TOOLCHAIN_VERSION"
   -a      Target cpu architecture. Default "$TARGET_ARCH"
                   armeabi     - tune for android arm v5
                   armeabi-v7a - tune for android arm v7
                   Optional name suffix: v5/v7
   -h      Shared
                   0 - Build static version of qt
                   1 - Build shared version of qt
   -x      Exceptions options
                   0 - don't use exceptions (you won't be able to build qtcreator with such a qt though)
                   1 - use exceptions
                   Optional name suffix: Xc/Nx
   -d      Build debug qt
                   Optional name suffix: D/R/DaR
   -r      Build release qt
                   Optional name suffix: R/D/DaR
   -m      Modify install path according to passed in options (name suffixes applied in above listed order).

   -i      Install path. Default "$DEST_DIR_QT"
EOF
}

INSTSUFFIX=""
CFGOPTIONS=""
while getopts "p:l:q:c:n:o:f:v:a:h:x:d:r:m:i:" arg; do
case $arg in
	p)
		help
		exit 0
		;;
	l)
		CLEAN_QT=$OPTARG
		;;
	q)
		CONFIGURE_QT=$OPTARG
		;;
	c)
		CLEAN_QT=$OPTARG
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
	r)
		RELEASE_QT=$OPTARG
		;;
	i)
		DEST_DIR_QT=$OPTARG
		;;
	m)
		MODIFY_DEST_DIR_QT=$OPTARG
		;;
	?)
		help
		exit 0
		;;
esac
done

if [ "$DEBUG_QT" -eq "1" -a "$RELEASE_QT" -eq "1" ]; then
	# This doesn't work, the release and debug builds use the same
	# temporary folders and end up with a release lib/dll that's really
	# a debug one.
	CFGOPTIONS="${CFGOPTIONS} -debug-and-release "
	INSTSUFFIX="${INSTSUFFIX}DaR"
else
	if [ "$DEBUG_QT" -eq "1" ]; then
		CFGOPTIONS="${CFGOPTIONS} -debug "
		INSTSUFFIX="${INSTSUFFIX}D"
	else
		CFGOPTIONS="${CFGOPTIONS} -release "
		INSTSUFFIX="${INSTSUFFIX}R"
	fi
fi

if [ "$MODIFY_DEST_DIR_QT" -eq "1" ]; then
	DEST_DIR_QT=${DEST_DIR_QT}-${INSTSUFFIX}
fi

echo New install of Qt will be to $DEST_DIR_QT

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

if [ "$CLEAN_QT" -eq "1" ]; then
	if [ -f Makefile ]; then
		make distclean
	fi
	if [ -d qmake ]; then
		pushd .
		cd qmake
		if [ -f Makefile ]; then
			make clean
		fi
		popd
	fi
fi

if [ $CONFIGURE_QT = 1 ]
then
	if [ $SRC_DIR_QT = $PWD ]
	then
		make confclean distclean
		rm -fr include
		rm -fr plugins
		rm -fr lib
		git checkout lib
		rm -fr imports
		git checkout imports
	fi

	$SRC_DIR_QT/configure -v -opensource -qpa -arch arm \
		-no-phonon -freetype -fast -xplatform android-g++ \
		$PLATFORM -host-little-endian \
		-little-endian -no-qt3support -no-largefile \
		-openssl -pch -reduce-relocations -reduce-exports \
		-nomake demos -nomake examples -confirm-license \
		$CFGOPTIONS -prefix $DEST_DIR_QT \
		-script || exit 1
fi

# This should loop until make succeeds, Workaround for Cygwin/MSYS
# couldn't commit heap memory error
make -j9
# make mocables wasn't being done on the latest official Git, so just in-case
# it's the same on android-lighthouse, I force it.
pushd .; cd /usr/Qt/Git/src/corelib; make mocables; make; popd
make -j 9
pushd .; cd /usr/Qt/Git/src/corelib; make mocables; make; popd
make -j 9

while [ "$?" != "0" ]
do
	if [ -f /usr/break-make ]; then
		echo "Detected break-make"
		rm -f /usr/break-make
		exit 1
	fi
	make -j9
done

if [ $PATCH_QT = 1 ]
then
    if [ "$OSTYPE" = "msys" ]; then
        $SRC_DIR_QT/qpatch.exe $SRC_DIR_QT/files-to-patch-android-mingw $DEST_DIR_QT $PWD
    else
        $SRC_DIR_QT/qpatch $SRC_DIR_QT/files-to-patch-android $DEST_DIR_QT $PWD
    fi
fi

#INSTALL_ROOT=$QT_SRC_DIR/qt/$TARGET_ARCH make install

make install
while [ "$?" != "0" ]
do
	if [ -f /usr/break-make ]; then
		echo "Detected break-make"
		rm -f /usr/break-make
		exit 1
	fi
	make install
done

$SRC_DIR_QT/copy-private-headers.sh $DEST_DIR_QT/private-headers

