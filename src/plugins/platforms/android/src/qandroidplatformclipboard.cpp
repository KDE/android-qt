/******************************************************************************
* Copyright 2012  BogDan Vatra <bog_dan_ro@yahoo.com>                         *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) version 3, or any           *
* later version accepted by the membership of KDE e.V. (or its                *
* successor approved by the membership of KDE e.V.), which shall              *
* act as a proxy defined in Section 6 of version 3 of the license.            *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public            *
* License along with this library. If not, see <http://www.gnu.org/licenses/>.*
******************************************************************************/

#include "qandroidplatformclipboard.h"
#include "androidjnimain.h"
#ifndef QT_NO_CLIPBOARD
#include <QMimeData>

QAndroidPlatformClipboard::QAndroidPlatformClipboard()
{
    QtAndroid::setClipboardListener(this);
}

QMimeData *QAndroidPlatformClipboard::mimeData(QClipboard::Mode mode)
{
    if (QClipboard::Clipboard != mode || !QtAndroid::hasClipboardText())
        return 0;
    QMimeData * mimeData = new QMimeData();
    mimeData->setText(QtAndroid::clipboardText());
    return mimeData;
}

void QAndroidPlatformClipboard::setMimeData(QMimeData *data, QClipboard::Mode mode)
{
    if (!data || !data->hasText() || QClipboard::Clipboard != mode)
        return;
    QtAndroid::setClipboardText(data->text());
}

bool QAndroidPlatformClipboard::supportsMode(QClipboard::Mode mode) const
{
    return QClipboard::Clipboard == mode;
}

#endif // QT_NO_CLIPBOARD
