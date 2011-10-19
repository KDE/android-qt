#!/bin/bash

mkdir -p $2
pushd $1
rm build.xml  *.properties
cp -a ../jar_14/* .
${ANDROID_SDK_TOOLS_PATH}android update lib-project -t android-14 -p .
${ANT_TOOL_PATH}ant clean release
${ANDROID_SDK_PLATFORM_TOOLS_PATH}dx --dex --output=$2/QtIndustrius-14.jar bin/classes

rm build.xml  *.properties
cp -a ../jar_8/* .
${ANDROID_SDK_TOOLS_PATH}android update lib-project -t android-8 -p .
${ANT_TOOL_PATH}ant clean release
${ANDROID_SDK_PLATFORM_TOOLS_PATH}dx --dex --output=$2/QtIndustrius-8.jar bin/classes

popd
