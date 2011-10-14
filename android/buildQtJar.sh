#!/bin/bash

mkdir -p $2
pushd $1
${ANDROID_SDK_TOOLS_PATH}android update lib-project -t android-13 -p .
${ANT_TOOL_PATH}ant clean compile
${ANDROID_SDK_PLATFORM_TOOLS_PATH}dx --dex --output=$2/QtIndustrius.jar bin/classes
popd
