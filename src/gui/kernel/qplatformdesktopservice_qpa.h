/*
I BogDan Vatra < bog_dan_ro@yahoo.com >, the copyright holder of this work,
hereby release it into the public domain. This applies worldwide.

In case this is not legally possible, I grant any entity the right to use
this work for any purpose, without any conditions, unless such conditions
are required by law.
*/

#ifndef QPLATFORMDESKTOPSERVICE_QPA_H
#define QPLATFORMDESKTOPSERVICE_QPA_H

#include <QDesktopServices>
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class Q_GUI_EXPORT QPlatformDesktopService
{
public:
    virtual ~QPlatformDesktopService() { }
    virtual QString displayName ( QDesktopServices::StandardLocation type );
    virtual bool openUrl ( const QUrl & url );
    virtual QString storageLocation ( QDesktopServices::StandardLocation type );
};

QT_END_NAMESPACE

QT_END_HEADER


#endif // QPLATFORMDESKTOPSERVICE_QPA_H
