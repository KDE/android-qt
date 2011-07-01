/*
I BogDan Vatra < bog_dan_ro@yahoo.com >, the copyright holder of this work,
hereby release it into the public domain. This applies worldwide.

In case this is not legally possible, I grant any entity the right to use
this work for any purpose, without any conditions, unless such conditions
are required by law.
*/

#ifndef QT_NO_DESKTOPSERVICES
#include <private/qapplication_p.h>
#include <QtGui/QPlatformDesktopService>

QT_BEGIN_NAMESPACE

QString QDesktopServices::storageLocation(StandardLocation type)
{
    return QApplicationPrivate::platformIntegration()->platformDesktopService()->storageLocation(type);
}

QString QDesktopServices::displayName(StandardLocation type)
{
    return QApplicationPrivate::platformIntegration()->platformDesktopService()->displayName(type);
}

QT_END_NAMESPACE

#endif
