#!/bin/bash -x

function createJar
{
    # param $1 JARs src path
    # param $2 JARs dest path
    # param $3 API level
    cp -a $1/* .
    cp -a $1/../jar_${3}/* .
    ${ANDROID_SDK_TOOLS_PATH}android update lib-project -t android-${3} -p .
    ${ANT_TOOL_PATH}ant clean release
    ${ANDROID_SDK_PLATFORM_TOOLS_PATH}dx --dex --output=$2/QtIndustrius-${3}.jar bin/classes
}

mkdir -p $2
TEMP_FOLDER=`mktemp -d tmp.XXXXXXXXXX`
pushd $TEMP_FOLDER

for apiLevel in 4 7 8 14
do
    createJar $1 $2 $apiLevel
done

popd

rm -fr $TEMP_FOLDER
