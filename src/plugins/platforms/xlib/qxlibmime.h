/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QTESTLITEMIME_H
#define QTESTLITEMIME_H

#include <private/qdnd_p.h>

#include <QtGui/QClipboard>

#include "qxlibintegration.h"
#include "qxlibclipboard.h"

class QXlibMime : public QInternalMimeData {
    Q_OBJECT
public:
    QXlibMime();
    ~QXlibMime();

    static QList<Atom> mimeAtomsForFormat(Display *display, const QString &format);
    static QString mimeAtomToString(Display *display, Atom a);
    static bool mimeDataForAtom(Display *display, Atom a, QMimeData *mimeData, QByteArray *data, Atom *atomFormat, int *dataFormat);
    static QStringList mimeFormatsForAtom(Display *display, Atom a);
    static Atom mimeStringToAtom(Display *display, const QString &mimeType);
    static QVariant mimeConvertToFormat(Display *display, Atom a, const QByteArray &data, const QString &format, QVariant::Type requestedType, const QByteArray &encoding);
    static Atom mimeAtomForFormat(Display *display, const QString &format, QVariant::Type requestedType, const QList<Atom> &atoms, QByteArray *requestedEncoding);
};

#endif // QTESTLITEMIME_H
