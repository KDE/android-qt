#!/bin/bash

# Warning :: this will take ages.

# Execute this from a folder where you want to build Qt for Android, e.g.
# having my Git repo in a folder called QtNecessitasSrc in the current directory:
# mkdir QtNecessitasBuild; cd QtNecessitasBuild; ../QtNecessitasSrc/build.sh

CLEAN_QT=0
CONFIGURE_QT=1
BUILD_QT=0
INSTALL_QT=0
SHARED_QT=1
RTTI_QT=0
EXECPTIONS_QT=1
DEBUG_QT=0
RELEASE_QT=1
MODIFY_DEST_DIR_QT=0

help()
{
cat << EOF
usage: $0 options

OPTIONS:
   -p      Shows this message
   -c      Clean qt
   -q      Configure qt
                   0 - don't configure qt
                   1 - configure qt
   -b      Build qt
                   0 - don't build
                   1 - build
   -h      Shared
                   0 - static version of qt
                   1 - shared version of qt
                   Optional name suffix: Sh/St
   -x      Exceptions options
                   0 - don't use exceptions (you won't be able to build qtcreator with such a qt though)
                   1 - use exceptions
                   Optional name suffix: Xc/Nx
   -d      Build debug qt
                   Optional name suffix: D/R/DaR
   -r      Build release qt
                   Optional name suffix: R/D/DaR
   -m      Modify install path according to passed in options (name suffixes applied in above listed order).
   -o      Old Qt directory (not needed?)
   -i      Install path. Default "$QT_INSTALL_DIR"
   -s      Qt source directory
EOF
}

INSTSUFFIX=""
CFGOPTIONS=""
while getopts "p:c:q:b:h:x:r:d:m:o:i:s:" arg; do
echo $arg $OPTARG
case $arg in
	p)
		help
		exit 0
		;;
	c)
		CLEAN_QT=$OPTARG
		echo "CLEAN $CLEAN_QT"
		;;
	q)
		CONFIGURE_QT=$OPTARG
		echo "CONFIGURE $CONFIGURE_QT"
		;;
	b)
		BUILD_QT=$OPTARG
		echo "BUILD $BUILD_QT"
		;;
	k)
		INSTALL_QT=$OPTARG
		echo "INSTALL $INSTALL_QT"
		;;
	h)
		SHARED_QT=$OPTARG
		echo "SHARED $SHARED_QT"
		if [ "$SHARED_QT" -eq "1" ]; then
			INSTSUFFIX="${INSTSUFFIX}Sh"
			CFGOPTIONS="${CFGOPTIONS} -shared "
		else
			INSTSUFFIX="${INSTSUFFIX}St"
			CFGOPTIONS="${CFGOPTIONS} -static "
		fi
		echo "SHARED $SHARED_QT"
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
		echo "EXCEPTIONS"
		;;
	d)
		DEBUG_QT=$OPTARG
		echo "DEBUG"
		;;
	r)
		RELEASE_QT=$OPTARG
		echo "RELEASE"
		;;
	o)
		OLD_QT=$OPTARG
		echo "OLD"
		;;
	i)
		DEST_DIR_QT=$OPTARG
		echo "NEW"
		;;
	s)
		SRC_DIR_QT=$OPTARG
		echo "SRC"
		;;
	m)
		MODIFY_DEST_DIR_QT=$OPTARG
		;;
	?)
		help
		exit 1
		;;
esac
done

if [ "$OSTYPE" = "msys" ]; then
        NDKHOST="windows"
        NDKDIR="C:/usr/android-sdk-windows/android-ndk-r5b"
#	NDKDIR="C:/Necessitas/android-ndk-r5b"
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
fi

# Makes {shared}*{debug,release}*{exceptions,noexceptions}, all armv7-a, and no thumb.
#`dirname $0`/androidconfigbuild.sh -l 1 -q 1 -n $NDKDIR -o $NDKHOST -f arm-linux-androideabi -v 4.4.3 -a armeabi-v7a \
#         -h 1 -x 1 -d 0 -r 1 -m 1 -i $DEST_DIR_QT
#`dirname $0`/androidconfigbuild.sh -l 1 -q 1 -n $NDKDIR -o $NDKHOST -f arm-linux-androideabi -v 4.4.3 -a armeabi-v7a \
#         -h 1 -x 1 -d 1 -r 0 -m 1 -i $DEST_DIR_QT
#Without exceptions get: QtXmlPatterns was requested, but it can't be built due to exceptions being disabled.
#`dirname $0`/androidconfigbuild.sh -l 1 -q 1 -n $NDKDIR -o $NDKHOST -f arm-linux-androideabi -v 4.4.3 -a armeabi-v7a \
#         -h 1 -x 0 -d 0 -r 1 -m 1 -i $DEST_DIR_QT
#`dirname $0`/androidconfigbuild.sh -l 1 -q 1 -n $NDKDIR -o $NDKHOST -f arm-linux-androideabi -v 4.4.3 -a armeabi-v7a \
#         -h 1 -x 0 -d 1 -r 0 -m 1 -i $DEST_DIR_QT

if [ "$BUILD_QT"="0" ]; then
	INSTALL_QT=0
fi

`dirname $0`/androidconfigbuild.sh -l $CLEAN_QT -q $CONFIGURE_QT -b $BUILD_QT -k $INSTALL_QT -c $INSTALL_QT -n $NDKDIR -o $NDKHOST -f arm-linux-androideabi -v 4.4.3 -a armeabi-v7a \
         -h 1 -x 1 -d $DEBUG_QT -r $RELEASE_QT -m 0 -i $DEST_DIR_QT
