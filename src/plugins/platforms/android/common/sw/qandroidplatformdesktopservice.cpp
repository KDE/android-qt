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

#include "qandroidplatformdesktopservice.h"
#include <QUrl>
#include <QDir>
#include <QDebug>

QAndroidPlatformDesktopService::QAndroidPlatformDesktopService()
{
    JNIEnv* env;
    if (QtAndroid::javaVM()->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return;
    }
    m_openURIMethodID=env->GetStaticMethodID(QtAndroid::applicationClass(), "openURL", "(Ljava/lang/String;)V");
}

bool QAndroidPlatformDesktopService::openUrl ( const QUrl & url )
{
    JNIEnv* env;
    if (QtAndroid::javaVM()->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }
    jstring string = env->NewString((const jchar*)url.toString().constData(), url.toString().length());
    env->CallStaticVoidMethod(QtAndroid::applicationClass(), m_openURIMethodID,string);
    env->DeleteLocalRef(string);
    return true;
}

QString QAndroidPlatformDesktopService::displayName ( QDesktopServices::StandardLocation type )
{
    Q_UNUSED(type);
    return QString();
}

static QByteArray getPath(const char * envVar)
{
    QByteArray path=qgetenv(envVar);
    return path.length()?path:QDir::homePath().toUtf8();
}

static QByteArray getPath(const char * envVar, const QByteArray & defaultValue)
{
    QByteArray path=qgetenv(envVar);
    return path.length()?path:defaultValue;
}

QString QAndroidPlatformDesktopService::storageLocation ( QDesktopServices::StandardLocation type )
{
    switch (type)
    {
        case QDesktopServices::DesktopLocation:
            return getPath("DESKTOP_PATH");
        case QDesktopServices::DocumentsLocation:
            return getPath("DOCUMENTS_PATH");
        case QDesktopServices::FontsLocation:
            return getPath("FONTS_PATH", "/system/fonts");
        case QDesktopServices::ApplicationsLocation:
            return getPath("APPLICATIONS_PATH", "/data/data");
        case QDesktopServices::MusicLocation:
            return getPath("MUSIC_PATH");
        case QDesktopServices::MoviesLocation:
            return getPath("MOVIES_PATH");
        case QDesktopServices::PicturesLocation:
            return getPath("PICTURES_PATH");
        case QDesktopServices::TempLocation:
            return QDir::tempPath();
        case QDesktopServices::HomeLocation:
            return QDir::homePath();
        case QDesktopServices::DataLocation:
            return getPath("DATA_PATH");
        case QDesktopServices::CacheLocation:
            return getPath("CACHE_PATH");
    }
    return QString();
}
