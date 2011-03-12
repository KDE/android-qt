#!/bin/bash

# This is awkward. The private headers are only relative includes to the actual files.
# So need to find all _p.h in $1 (e.g. /usr/Qt/MinGW), then, for each one, read it
# and take sed the filename out of it, then copy that file into $2.

DEBUG=0
FILENAMESDIR=$1
DSTDIR=$2

if [ "$FILENAMESDIR" = "" ]; then
	echo "Please pass in the build-tree relative src directory to find *_p.h in (usually include) $FILENAMESDIR"
	exit 1
fi

if [ "$DSTDIR" = "" ]; then
	echo "Please pass in the dst directory (e.g. C:/Qt/MinGW-Git/private-headers)"
	exit 1
fi

echo "Copying private headers (#included files in) :: ${FILENAMESDIR} -> ${DSTDIR}"

pushd .
cd $FILENAMESDIR
FILENAMESDIRABS=`pwd`
FILES=`find . -name "*_p.h" -exec echo {} ';'`
for FILE in $FILES; do
	REALFILE=`awk '{ print $2 }' ${FILE}`
	REALFILE=`echo $REALFILE | tr -d '"'`
	DIRNAME=`dirname $FILE`

	pushd . > /dev/null
	cd $DIRNAME
	SRCFILE=${FILENAMESDIRABS}/${FILE}
	if [ "$DEBUG" = "1" ]; then
		echo "Found #include of $REALFILE in $FILE, copying it DSTDIR=$DSTDIR / FILENAMESDIR=$FILENAMESDIR / DIRNAME=$DIRNAME"
	fi
	mkdir -p $DSTDIR/$FILENAMESDIR/$DIRNAME/
	cp -f $REALFILE $DSTDIR/$FILENAMESDIR/$DIRNAME/
	popd  > /dev/null
done
popd
