/*
I BogDan Vatra < bog_dan_ro@yahoo.com >, the copyright holder of this work,
hereby release it into the public domain. This applies worldwide.

In case this is not legally possible, I grant any entity the right to use
this work for any purpose, without any conditions, unless such conditions
are required by law.
*/

#include "qplatformdesktopservice_qpa.h"

/*!
Documentation needed
*/

QString QPlatformDesktopService::storageLocation ( QDesktopServices::StandardLocation /*type*/ )
{
    return QString();
}

/*!
Documentation needed
*/
QString QPlatformDesktopService::displayName ( QDesktopServices::StandardLocation /*type*/ )
{
    return QString();
}

bool QPlatformDesktopService::openUrl ( const QUrl & /*url */)
{
    return false;
}
