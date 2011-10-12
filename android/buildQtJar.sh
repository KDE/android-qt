#!/bin/bash

mkdir -p $2
pushd $1
android update lib-project -t android-13 -p .
ant clean compile
dx --dex --output=$2/QtIndustrius.jar bin/classes
popd
