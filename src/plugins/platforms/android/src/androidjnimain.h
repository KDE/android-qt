/*
    Copyright (c) 2009-2011, BogDan Vatra <bog_dan_ro@yahoo.com>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the  BogDan Vatra <bog_dan_ro@yahoo.com> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY BogDan Vatra <bog_dan_ro@yahoo.com> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL BogDan Vatra <bog_dan_ro@yahoo.com> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ANDROID_APP_H
#define ANDROID_APP_H

#ifdef ANDROID_PLUGIN_OPENGL
#include <EGL/eglplatform.h>
#endif

#include <jni.h>
#include "native/include/android/asset_manager.h"


class QImage;
class QRect;
class QPoint;
class QThread;
class QAndroidPlatformIntegration;
class QMenuBar;
class QMenu;
class QWidget;

namespace QtAndroid
{
    typedef union {
        JNIEnv* nativeEnvironment;
        void* venv;
    } UnionJNIEnvToVoid;

    void setAndroidPlatformIntegration(QAndroidPlatformIntegration * androidGraphicsSystem);
    void setQtThread(QThread * thread);

    void setFullScreen(QWidget * widget);

    // Software keyboard support
    void showSoftwareKeyboard(int top, int left, int width, int height, int inputHints);
    void resetSoftwareKeyboard();
    void hideSoftwareKeyboard();
    // Software keyboard support

    // Menu support
    void showOptionsMenu();
    void hideOptionsMenu();

    void showContextMenu();
    void hideContextMenu();
    // Menu support


#ifndef ANDROID_PLUGIN_OPENGL
    void flushImage(const QPoint & pos, const QImage & image, const QRect & rect);
#else
    EGLNativeWindowType getNativeWindow(bool waitToCreate=true);
#endif

    JavaVM * javaVM();
    jclass findClass(const QString & className, JNIEnv * env);
    AAssetManager * assetManager();
    jclass applicationClass();
}
#endif // ANDROID_APP_H
