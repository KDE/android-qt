#!/bin/bash


# Redundant!

# Warning :: this will take ages.

# Execute this from a folder where you want to build Qt for Android

# TODO :: Add options for avr and 64bit.

CLEAN=1
CONFIGURE=1
BUILDRELEASE=0
BUILDDEBUG=0

while getopts "p:c:q:d:r:" arg; do
	echo "arg is $arg $OPTARG"
	case $arg in
		p)
			`dirname $0`/build-mingw.sh -p
			exit 0
			;;
		c)
			CLEAN=$OPTARG
			;;
		q)
			CONFIGURE=$OPTARG
			;;
		r)
			BUILDRELEASE=$OPTARG
			;;
		d)
			BUILDDEBUG=$OPTARG
			;;
	esac
done

if [ "$CLEAN" == "1" ]; then
	CONFIGURE=1
fi

# Rationale for the below is ::
# Static probably doesn't work.
# RTTI is probably not needed.
# Exceptions are needed for QtCreator (botan).

if [ "$BUILDRELEASE" == "1" -a "$BUILDDEBUG" == "1" ]; then
	echo "Error, debug and release build in the same place == bad"
	exit 1
fi

# There's a bug somewhere here, both debug and release get built?
if [ "$BUILDRELEASE" == "1" ]; then
	`dirname $0`/build-mingw.sh -c $CLEAN -q $CONFIGURE -h 1 -x 1 -r 1 -d 0 -m 1 -o C:/Qt/4.7.2-official -s /usr/Qt/Git -i C:/Necessitas/4.8.0
fi
if [ "$BUILDDEBUG" == "1" ]; then
	`dirname $0`/build-mingw.sh -c $CLEAN -q $CONFIGURE -h 1 -x 1 -r 0 -d 1 -m 1 -o C:/Qt/4.7.2-official -s /usr/Qt/Git -i C:/Necessitas/4.8.0
fi
