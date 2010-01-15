/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qtextcodecplugin.h>
#include <qtextcodec.h>
#include <qstringlist.h>

#ifndef QT_NO_TEXTCODECPLUGIN

#include "qeucjpcodec.h"
#include "qjiscodec.h"
#include "qsjiscodec.h"
#ifdef Q_WS_X11
#include "qfontjpcodec.h"
#endif

QT_BEGIN_NAMESPACE

class JPTextCodecs : public QTextCodecPlugin
{
public:
    JPTextCodecs() {}

    QList<QByteArray> names() const;
    QList<QByteArray> aliases() const;
    QList<int> mibEnums() const;

    QTextCodec *createForMib(int);
    QTextCodec *createForName(const QByteArray &);
};

QList<QByteArray> JPTextCodecs::names() const
{
    QList<QByteArray> list;
    list += QEucJpCodec::_name();
    list += QJisCodec::_name();
    list += QSjisCodec::_name();
#ifdef Q_WS_X11
    list += QFontJis0201Codec::_name();
    list += QFontJis0208Codec::_name();
#endif
    return list;
}

QList<QByteArray> JPTextCodecs::aliases() const
{
    QList<QByteArray> list;
    list += QEucJpCodec::_aliases();
    list += QJisCodec::_aliases();
    list += QSjisCodec::_aliases();
#ifdef Q_WS_X11
    list += QFontJis0201Codec::_aliases();
    list += QFontJis0208Codec::_aliases();
#endif
    return list;
}

QList<int> JPTextCodecs::mibEnums() const
{
    QList<int> list;
    list += QEucJpCodec::_mibEnum();
    list += QJisCodec::_mibEnum();
    list += QSjisCodec::_mibEnum();
#ifdef Q_WS_X11
    list += QFontJis0201Codec::_mibEnum();
    list += QFontJis0208Codec::_mibEnum();
#endif
    return list;
}

QTextCodec *JPTextCodecs::createForMib(int mib)
{
    if (mib == QEucJpCodec::_mibEnum())
        return new QEucJpCodec;
    if (mib == QJisCodec::_mibEnum())
        return new QJisCodec;
    if (mib == QSjisCodec::_mibEnum())
        return new QSjisCodec;
#ifdef Q_WS_X11
    if (mib == QFontJis0208Codec::_mibEnum())
        return new QFontJis0208Codec;
    if (mib == QFontJis0201Codec::_mibEnum())
        return new QFontJis0201Codec;
#endif
    return 0;
}


QTextCodec *JPTextCodecs::createForName(const QByteArray &name)
{
    if (name == QEucJpCodec::_name() || QEucJpCodec::_aliases().contains(name))
        return new QEucJpCodec;
    if (name == QJisCodec::_name() || QJisCodec::_aliases().contains(name))
        return new QJisCodec;
    if (name == QSjisCodec::_name() || QSjisCodec::_aliases().contains(name))
        return new QSjisCodec;
#ifdef Q_WS_X11
    if (name == QFontJis0208Codec::_name() || QFontJis0208Codec::_aliases().contains(name))
        return new QFontJis0208Codec;
    if (name == QFontJis0201Codec::_name() || QFontJis0201Codec::_aliases().contains(name))
        return new QFontJis0201Codec;
#endif
    return 0;
}

Q_EXPORT_STATIC_PLUGIN(JPTextCodecs);
Q_EXPORT_PLUGIN2(qjpcodecs, JPTextCodecs);

QT_END_NAMESPACE

#endif // QT_NO_TEXTCODECPLUGIN
