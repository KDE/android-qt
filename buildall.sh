#!/bin/bash

CURRENT_PATH=$PWD
QT_SRC_DIR=`dirname $0`
QT_SRC_DIR=`(cd "$QT_SRC_DIR"; /bin/pwd)`

for architecture in "armeabi" #"armeabi-v7a"
do
#	mkdir $architecture
#	pushd $architecture
	$QT_SRC_DIR/androidconfigbuild.sh -c 0 -q 1 -a $architecture || exit 0
#	popd
done
